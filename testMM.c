#include<stdio.h>
#include<stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "segmentTable.h"
#include "pcb.h"
#include "frameTable.h"
#include "configuration.h"

FrameTable frameTable;
segmentTable* GDTptr;

PCB pcbArr[1];

int main()
{
	printf("Starting execution of simulator\n");
	int addr[100],n=8,i;

	// pageTable* level1PageTableptr = (pageTable*) calloc(1, sizeof(pageTable));
	// pageTable* level2PageTableptr = (pageTable*) calloc(1, sizeof(pageTable));
	// pageTable* level3PageTableptr = (pageTable*) calloc(1, sizeof(pageTable));

	// pageTable level1PageTable = *level1PageTableptr;
	// pageTable level2PageTable = *level2PageTableptr;
	// pageTable level3PageTable = *level3PageTableptr;

	// frameOfPageTable* level1PageTableFramesptr = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE, sizeof(frameOfPageTable));
	// frameOfPageTable* level2PageTableFramesptr = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE, sizeof(frameOfPageTable));
	// frameOfPageTable* level3PageTableFramesptr = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE, sizeof(frameOfPageTable));

	// level1PageTable.frames = level1PageTableFramesptr;
	// level2PageTable.frames = level2PageTableFramesptr;
	// level3PageTable.frames = level3PageTableFramesptr;

	segmentTable* LDTptr;

	// pageTable *level1PageTableptr, *level2PageTableptr, *level3PageTableptr;
	// frameOfPageTable *level1PageTableFramesptr, *level2PageTableFramesptr, *level3PageTableFramesptr;


	printf("Page table objects declared and frames alloted to them\n");


	for(i=0;i<n;i++)
	{
		scanf("%x",&addr[i]);
	}

	initFrameTable();

	initSegTable(GDTptr);

	initSegTable(LDTptr);
	
	// initPageTable(level3PageTable,level2PageTable,level1PageTable);

	// initPCB(pcbArr[0],&level3PageTable);

	// initPageTable(level3PageTableptr,level2PageTableptr,level1PageTableptr,level3PageTableFramesptr,level2PageTableFramesptr,level1PageTableFramesptr);

	initPCB(&pcbArr[0],LDTptr);

	// Set up done. Start simulation
	// allocateFrame(1,level1PageTable,0,1);
	// allocateFrame(1,level1PageTable,1,1);	//need to enter in page tables too
	//allocateFrame(1,1);


	i = 0;
	unsigned int* pageNo = calloc(1, sizeof(unsigned int));
	unsigned int* level = calloc(1,sizeof(unsigned int));
	pageTable** ptrToPageFaultPageTable;
	while(i < n)
	{
		if(searchPageTable(pcbArr[0].LDTPointer->entries[0].level3PageTableptr, ptrToPageFaultPageTable, addr[i],pageNo,level) == -1){
			printf("After returing to testMM searchPageTable: level = %d, pageFaultPageNumber = %d\n",*level,*pageNo);
			/*
			if(*level == 0){
				allocateFrame(1,&level1PageTable, *pageNo, *level);	
			}
			else if(*level == 1){
				allocateFrame(1,&level2PageTable, *pageNo, *level);	
				printf("\nMain:the present bit =%d\n",level2PageTable.frames[*pageNo/256].entries[*pageNo%256].present);

			}
			else{
				allocateFrame(1,&level3PageTable, *pageNo, *level);
				printf("\nMain:the present bit =%d\n",level3PageTable.frames[*pageNo/256].entries[*pageNo%256].present);

			}*/

			allocateFrame(1,*ptrToPageFaultPageTable, *pageNo, *level);	
			// We need to retry for this address
			printf("Retrying for same page after page fault\n");
			printf("------------------------------------\n");

			continue;
		}
		else{
			printf("Search in page table was successful\n");
			i++;		
		}
				printf("------------------------------------\n");

	}

	// free(level1PageTableptr);
	// free(level2PageTableptr);
	// free(level3PageTableptr);
	// free(level1PageTableFramesptr);
	// free(level2PageTableFramesptr);
	// free(level3PageTableFramesptr);

	deleteProcess(0);
	free(pageNo);
	free(level);
	return 0;
}