#include<stdio.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "frameTable.h"

extern FrameTable frameTable;
extern FILE *outputFile;


/*
Returns a replacable frame for allocation or -1 if no frame is available
*/
int getReplacableEmptyFrame()
{
	int i;
	for(i=0;i<NUM_FRAMES;i++)
	{
		if(frameTable.entries[i].emptyBit==0 && frameTable.entries[i].considerInLfu == 1&& frameTable.entries[i].lock==0)
			return i;
	}
	return -1; // returns -1 if no empty frame available
}



/*
Returns an irreplacable frame for allocation or -1 if no frame is available
*/
int getNonReplaceableFrame()
{
	int i;
	for(i=0;i<NUM_FRAMES;i++)
	{
		if(frameTable.entries[i].emptyBit==0 && frameTable.entries[i].considerInLfu == 0 )
			return i;
	}
	return -1; // returns -1 if no empty frame available
}




/*
Updates the frame access count which is used in LFU page replacement
in the frame table
*/
int updateLfuCount(int frameNo)
{
	fprintf(outputFile, "Updating count for frame# %d\n",frameNo);
	if(frameNo>=NUM_FRAMES)
		return -1;
	frameTable.entries[frameNo].LfuCount.value++;
	//fprintf(outputFile, "count = %d\n",frameTable.entries[frameNo].LfuCount.value);
	return 1;
}




/*
Returns the LFU frame to use for replacement
*/
int getLfuFrameNum()
{
	int minCount = frameTable.entries[0].LfuCount.value, indexFrame=0;
	int i;

	for(i=0;i<NUM_FRAMES;i++)
	{
		if(frameTable.entries[i].considerInLfu==1 && frameTable.entries[i].LfuCount.value<minCount)
		{
			minCount = frameTable.entries[i].LfuCount.value;
			indexFrame = i;
		}
	}
	//fprintf(outputFile, "minCount=%d|||frameno=%d\n",minCount,indexFrame );
	return indexFrame;
}




/*
Returns the value of the dirty bit of the given frame number
*/
short unsigned getDirtyBitFrameTable(int index)
{
	return frameTable.entries[index].dirtyBit;
}




/*
Sets the value of the dirty bit of the given frame number 
to the value given as input by user
*/
short unsigned setDirtyBitFrameTable(int index, int value)
{
	frameTable.entries[index].dirtyBit = value;	
}




/*
Returns the value of the empty bit of the given frame number
*/
short unsigned getEmptyBitFrameTable(int index)
{
	return frameTable.entries[index].emptyBit;
}




/*
Sets the value of the empty bit of the given frame number 
to the value given as input by user
*/
short unsigned setEmptyBitFrameTable(int index, int value)
{
	frameTable.entries[index].emptyBit=value;

}




/*
Runs the LFU frame ageing alogrithm 
*/
int frameAgeing()
{
	int i;
	for(i=0;i<NUM_FRAMES;i++)
	{
		frameTable.entries[i].LfuCount.value>>=1;
	}
	return 1;
}




/*
Finds an empty frame and allocates it to a process.
Takes segment number, page table level in which page fault occured and page index in the pafge table as input 
*/
int allocateFrame(int pid,int segno,pageTable *pT,int pageNum,int level)
{
	int frameNo = getReplacableEmptyFrame();

	//fprintf(outputFile, "pid = %d,segno=%d\n",pid,segno );

	if(frameNo!=-1)
	{	//placement
		fprintf(outputFile, "Frame allocated:%d(placement)\n",frameNo);
		frameTable.entries[frameNo].pageNum.value=pageNum;
		frameTable.entries[frameNo].pid=pid;
		frameTable.entries[frameNo].LfuCount.value=0;
		frameTable.entries[frameNo].dirtyBit=0;
		frameTable.entries[frameNo].emptyBit=1; 
		frameTable.entries[frameNo].considerInLfu=1;
		frameTable.entries[frameNo].level = level + 1;
		frameTable.entries[frameNo].segNum = segno;			//Confirm once if this is correct
	}
	else
	{
		//replacement
		fprintf(outputFile, "No free frame available in memory. Going for replacement\n");
		frameNo = getLfuFrameNum();

		//swap the page 
		if(frameTable.entries[frameNo].dirtyBit==1)
		{
			//func to swap page out
		}

		//
		//fprintf(outputFile, "segno:%d,pn:%d\n",frameTable.entries[frameNo].segNum,frameTable.entries[frameNo].pageNum);
		pageTable* pT2 = getPageTableFromPid(frameTable.entries[frameNo].pid,frameTable.entries[frameNo].segNum,frameTable.entries[frameNo].level);
		
		//fprintf(outputFile, "pt ref\n");
		updatePageTablePresentBit(pT2,frameTable.entries[frameNo].pageNum.value,0);

		//allocate the frame
		fprintf(outputFile, "Frame allocated:%d(replacement)",frameNo);

		frameTable.entries[frameNo].pageNum.value=pageNum;
		frameTable.entries[frameNo].pid=pid;
		frameTable.entries[frameNo].LfuCount.value=0;
		frameTable.entries[frameNo].dirtyBit=0;
		frameTable.entries[frameNo].emptyBit=1; 
		frameTable.entries[frameNo].considerInLfu=1;
		frameTable.entries[frameNo].level = level + 1;	
		frameTable.entries[frameNo].segNum = segno;			//Confirm once if this is correct
	//Here too
	}

	//update page table
	fprintf(outputFile, "Updating page table entry of page# %d\n",pageNum);

	updatePageTablePresentBit(pT,pageNum,1);
	//fprintf(outputFile, "\nthe updted present bit =%d\n",pT->frames[pageNum/256].entries[pageNum%256].present);
	setLock(frameNo);
	setFrameNo(pT, pageNum, frameNo);
	return frameNo;
}




/*setLock
Writes a dirty frame back to disk and sets its empty bit to zero.
*/
int invalidateFrame(int frameNo)
{
	if(frameTable.entries[frameNo].dirtyBit==1)
	{
		fprintf(outputFile, "Frame# %d is dirty. Writing it back to disk\n",frameNo);

		//write back to disk
	}
	frameTable.entries[frameNo].emptyBit=0;
	return 1;
}




/*
Initializes the ADT of the frame table
*/
int initFrameTable()
{
	int i;
	for(i=0;i<NUM_FRAMES;i++)
	{
		frameTable.entries[i].emptyBit=0;
		frameTable.entries[i].considerInLfu = 1;
		frameTable.entries[i].lock = 0;
	}
	//mark some frames as non-replaceable
	for(i=NUM_FRAMES-NUM_NON_REPLACABLE_FRAMES;i<NUM_FRAMES;i++){
		frameTable.entries[i].considerInLfu = 0;
	}

	//Allocate irreplacable frames for Frame Table
	for(i = NUM_FRAMES-NUM_NON_REPLACABLE_FRAMES; i < NUM_FRAMES-NUM_NON_REPLACABLE_FRAMES + 512; i++){
		frameTable.entries[i].emptyBit = 1;
	}

}
int setLock(int frameNo)
{

	frameTable.entries[frameNo].lock=1;
}

int readFromMemory(int frameNo)
{
	if(frameTable.entries[frameNo].lock==1)
		frameTable.entries[frameNo].lock = 0;

	updateLfuCount(frameNo);

}

int writeToMemory(int frameNo)
{
	if(frameTable.entries[frameNo].lock==1)
		frameTable.entries[frameNo].lock = 0;

	setDirtyBitFrameTable(frameNo, 1);

	updateLfuCount(frameNo);
}