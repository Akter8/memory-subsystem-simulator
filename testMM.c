#include <stdio.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "frameTable.h"
#include "configuration.h"
FrameTable frameTable;

int main()
{
	int addr[100],n=100,i;
	pageTable level1PageTable;
	pageTable level2PageTable;
	pageTable level3PageTable;

	frameOfPageTable level1PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE];
	frameOfPageTable level2PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE];
	frameOfPageTable level3PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE];

	level1PageTable.frames = level1PageTableFrames;
	level2PageTable.frames = level2PageTableFrames;
	level3PageTable.frames = level3PageTableFrames;


	for(i=0;i<n;i++)
	{
		scanf("%d",&addr[i]);
	}

	initFrameTable();
	
	initPageTable(level3PageTable,level2PageTable,level1PageTable);
	allocateFrame(1,level1PageTable,0,1);
	allocateFrame(1,level1PageTable,1,1);	//need to enter in page tables too
	//allocateFrame(1,1);


	for(i=0;i<n;i++)
	{
		int pageNo;
		int level;
		if(searchPageTable(level3PageTable, level2PageTable, level1PageTable, addr[i],&pageNo, &level)!=-1){
			if(level == 0){
				allocateFrame(1,level1PageTable, pageNo, level);	
			}
			else if(level == 1){
				allocateFrame(1,level2PageTable, pageNo, level);	
			}
			else{
				allocateFrame(1,level3PageTable, pageNo, level);
			}
			
		}

	}
}