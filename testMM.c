#include <stdio.h>
#include "dataTypes.h"
#include "frameTable.h"
#include "pageTable.h"
#include "configuration.h"

pageTable level1PageTable;
pageTable level2PageTable;
pageTable level3PageTable;

FrameTable frameTable;

int main()
{
	int addr[100],n=20,i;

	for(i=0;i<n;i++)
	{
		scanf("%d",&addr[i]);
	}

	frameOfPageTable level1PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE];
	frameOfPageTable level2PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE];
	frameOfPageTable level3PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE];

	level1PageTable.frames = level1PageTableFrames;
	level2PageTable.frames = level2PageTableFrames;
	level3PageTable.frames = level3PageTableFrames;
	initFrameTable();
	
	printf("page table initialised");
	initPageTable();
	allocateFrame(1,0);	//need to enter in page tables too
	allocateFrame(1,1);


	for(i=0;i<n;i++)
	{
		int pageNo,level;
		if(searchPageTable(addr[i],&pageNo,&level)!=-1){
			
			allocateFrame(1,1,pageNo,level+1);
		}

	}
}