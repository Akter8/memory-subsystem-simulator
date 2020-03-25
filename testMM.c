#include <stdio.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "frameTable.h"
#include "pcb.h"
#include "configuration.h"

FrameTable frameTable;

PCB pcbArr[1];

int main()
{
	printf("Starting execution of simulator\n");
	int addr[100],n=30,i;
	pageTable level1PageTable;
	pageTable level2PageTable;
	pageTable level3PageTable;

	frameOfPageTable level1PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE];
	frameOfPageTable level2PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE];
	frameOfPageTable level3PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE];

	level1PageTable.frames = level1PageTableFrames;
	level2PageTable.frames = level2PageTableFrames;
	level3PageTable.frames = level3PageTableFrames;

	printf("Page table objects declared and frames alloted to them\n");


	for(i=0;i<n;i++)
	{
		scanf("%x",&addr[i]);
	}

	initFrameTable();
	
	initPageTable(level3PageTable,level2PageTable,level1PageTable);

	initPCB(pcbArr[0],&level3PageTable);

	// Set up done. Start simulation
	// allocateFrame(1,level1PageTable,0,1);
	// allocateFrame(1,level1PageTable,1,1);	//need to enter in page tables too
	//allocateFrame(1,1);


	i = 0;
	while(i < n)
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
			// We need to retry for this address
			continue;
		}
		else{
			printf("Search in page table was successful\n");
			i++;		
		}
	}
	return 0;
}