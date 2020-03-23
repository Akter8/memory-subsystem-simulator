#include "dataTypes.h"
#include "frameTable.h"
#include "pageTable.h"
#include<stdio.h>
#define NUM_NON_REPLACABLE_FRAMES 10
extern FrameTable frameTable;

int findEmptyFrame()
{
	int i;
	for(i=0;i<NUM_FRAMES;i++)
	{
		if(frameTable.entries[i].emptyBit==0)
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
int allocateFrame(int pid,int segNo,int pageNum,int level)
{
	int frameNo = findEmptyFrame();

	if(frameNo!=-1)
	{	//placement
		printf("\nFrame allocated:%d(placement)",frameNo);
		frameTable.entries[frameNo].pageNum.value=pageNum;
		frameTable.entries[frameNo].pid=pid;
		frameTable.entries[frameNo].LfuCount.value=0;
		frameTable.entries[frameNo].dirtyBit=0;
		frameTable.entries[frameNo].emptyBit=1; 
		frameTable.entries[frameNo].considerInLfu=1;
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

		updatePageTablePresentBit(frameTable.entries[frameNo].pid,frameTable.entries[frameNo].segNo,frameTable.entries[frameNo].pageNum.value,0,1);

		//allocate the frame
		printf("\nFrame allocated:%d(replacement)",frameNo);

		frameTable.entries[frameNo].pageNum.value=pageNum;
		frameTable.entries[frameNo].pid=pid;
		frameTable.entries[frameNo].LfuCount.value=0;
		frameTable.entries[frameNo].dirtyBit=0;
		frameTable.entries[frameNo].emptyBit=1; 
		frameTable.entries[frameNo].considerInLfu=1;

		//update page table
		updatePageTablePresentBit(pid,segNo,pageNum,1,level);

	}
}
int getNonReplaceableFrame()
{

}
int invalidateFrame(int frameNo)
{
	if(frameTable.entries[frameNo].dirtyBit==1)
	{
		//write back to disk
	}
	frameTable.entries[frameNo].emptyBit=0;
	return 1;
}

int initFrameTable()
{
	int i;
	for(i=0;i<NUM_FRAMES-NUM_NON_REPLACABLE_FRAMES;i++)
	{
		frameTable.entries[i].emptyBit=0;
	}
	//mark some frames as non-replaceable
	for(i=NUM_FRAMES-NUM_NON_REPLACABLE_FRAMES;i<NUM_FRAMES;i++)
		frameTable.entries[i].emptyBit=1;

}
