#include<stdio.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "frameTable.h"

extern FrameTable frameTable;

int getReplacableEmptyFrame()
{
	int i;
	for(i=0;i<NUM_FRAMES;i++)
	{
		if(frameTable.entries[i].emptyBit==0 && frameTable.entries[i].considerInLfu == 1)
			return i;
	}
	return -1; // returns -1 if no empty frame available
}
int updateLfuCount(int frameNo)
{
	if(frameNo>=NUM_FRAMES)
		return -1;
	frameTable.entries[frameNo].LfuCount.value++;
	return 1;
}
int getLfuFrameNum()
{
	int minCount = frameTable.entries[0].LfuCount.value, indexFrame=0;
	int i;

	for(i=0;i<NUM_FRAMES;i++)
	{
		if(frameTable.entries[i].considerInLfu==1 && frameTable.entries[0].LfuCount.value<minCount)
		{
			minCount = frameTable.entries[0].LfuCount.value;
			indexFrame = i;
		}
	}
	
	return indexFrame;
}
short unsigned getDirtyBitFrameTable(int index)
{
	return frameTable.entries[index].dirtyBit;
}
short unsigned setDirtyBitFrameTable(int index, int value)
{
	frameTable.entries[index].dirtyBit = value;	
}
short unsigned getEmptyBitFrameTable(int index)
{
	return frameTable.entries[index].emptyBit;
}
short unsigned setEmptyBitFrameTable(int index, int value)
{
	frameTable.entries[index].emptyBit=value;

}


int frameAgeing()
{
	int i;
	for(i=0;i<NUM_FRAMES;i++)
	{
		frameTable.entries[i].LfuCount.value>>=1;
	}
	return 1;
}



int allocateFrame(int pid, pageTable pT,int pageNum,int level)
{
	int frameNo = getReplacableEmptyFrame();

	if(frameNo!=-1)
	{	//placement
		printf("\nFrame allocated:%d(placement)",frameNo);
		frameTable.entries[frameNo].pageNum.value=pageNum;
		frameTable.entries[frameNo].pid=pid;
		frameTable.entries[frameNo].LfuCount.value=0;
		frameTable.entries[frameNo].dirtyBit=0;
		frameTable.entries[frameNo].emptyBit=1; 
		frameTable.entries[frameNo].considerInLfu=1;
		frameTable.entries[frameNo].level = level + 1;			//Confirm once if this is correct
	}
	else
	{
		//replacement
		frameNo = getLfuFrameNum();

		//swap the page 
		if(frameTable.entries[frameNo].dirtyBit==1)
		{
			//func to swap page out
		}

		//
		pageTable pT2 = getPageTableFromPid(frameTable.entries[frameNo].pid,frameTable.entries[frameNo].segNum,frameTable.entries[frameNo].level);
		updatePageTablePresentBit(pT2,frameTable.entries[frameNo].pageNum.value,0);

		//allocate the frame
		printf("\nFrame allocated:%d(replacement)",frameNo);

		frameTable.entries[frameNo].pageNum.value=pageNum;
		frameTable.entries[frameNo].pid=pid;
		frameTable.entries[frameNo].LfuCount.value=0;
		frameTable.entries[frameNo].dirtyBit=0;
		frameTable.entries[frameNo].emptyBit=1; 
		frameTable.entries[frameNo].considerInLfu=1;
		frameTable.entries[frameNo].level = level + 1;		//Here too

		//update page table
		updatePageTablePresentBit(pT,pageNum,1);

	}
}
int getNonReplaceableFrame()
{
	int i;
	for(i=0;i<NUM_FRAMES;i++)
	{
		if(frameTable.entries[i].emptyBit==0 && frameTable.entries[i].considerInLfu == 0)
			return i;
	}
	return -1; // returns -1 if no empty frame available
}
int invalidateFrame(int frameNo)
{
	if(frameTable.entries[frameNo].dirtyBit==1)
	{
		printf("Frame# %d is dirty. Writing it back to disk\n",frameNo);

		//write back to disk
	}
	frameTable.entries[frameNo].emptyBit=0;
	return 1;
}

int initFrameTable()
{
	int i;
	for(i=0;i<NUM_FRAMES;i++)
	{
		frameTable.entries[i].emptyBit=0;
		frameTable.entries[i].considerInLfu = 1;
	}
	//mark some frames as non-replaceable
	for(i=NUM_FRAMES-NUM_NON_REPLACABLE_FRAMES;i<NUM_FRAMES;i++){
		frameTable.entries[i].considerInLfu = 0;
	}

	//Allocate irreplacable frames for PCBs, Segment Tables and Frame Table




	//

}
