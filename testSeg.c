#include<stdio.h>
#include<stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "frameTable.h"
#include "configuration.h"
#include "segmentTable.h"
#include "pcb.h"

FrameTable frameTable;

PCB pcbArr[1];
segmentTable LDT,GDT;
int main()
{
	printf("Starting execution of simulator\n");
	long int addr[100],n=15,i;
	/*
	pageTable* level1PageTableptr = (pageTable*) calloc(1, sizeof(pageTable));
	pageTable* level2PageTableptr = (pageTable*) calloc(1, sizeof(pageTable));
	pageTable* level3PageTableptr = (pageTable*) calloc(1, sizeof(pageTable));

	pageTable level1PageTable = *level1PageTableptr;
	pageTable level2PageTable = *level2PageTableptr;
	pageTable level3PageTable = *level3PageTableptr;

	frameOfPageTable* level1PageTableFramesptr = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE, sizeof(frameOfPageTable));
	frameOfPageTable* level2PageTableFramesptr = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE, sizeof(frameOfPageTable));
	frameOfPageTable* level3PageTableFramesptr = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE, sizeof(frameOfPageTable));

	level1PageTable.frames = level1PageTableFramesptr;
	level2PageTable.frames = level2PageTableFramesptr;
	level3PageTable.frames = level3PageTableFramesptr;

	*/

	pageTable* level1PageTableptrSeg0 = (pageTable*) calloc(1, sizeof(pageTable));
	pageTable* level2PageTableptrSeg0 = (pageTable*) calloc(1, sizeof(pageTable));
	pageTable* level3PageTableptrSeg0 = (pageTable*) calloc(1, sizeof(pageTable));

	pageTable level1PageTableSeg0 = *level1PageTableptrSeg0;
	pageTable level2PageTableSeg0 = *level2PageTableptrSeg0;
	pageTable level3PageTableSeg0 = *level3PageTableptrSeg0;

	frameOfPageTable* level1PageTableFramesptrSeg0 = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE, sizeof(frameOfPageTable));
	frameOfPageTable* level2PageTableFramesptrSeg0 = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE, sizeof(frameOfPageTable));
	frameOfPageTable* level3PageTableFramesptrSeg0 = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE, sizeof(frameOfPageTable));

	level1PageTableSeg0.frames = level1PageTableFramesptrSeg0;
	level2PageTableSeg0.frames = level2PageTableFramesptrSeg0;
	level3PageTableSeg0.frames = level3PageTableFramesptrSeg0;



	pageTable* level1PageTableptrSeg1 = (pageTable*) calloc(1, sizeof(pageTable));
	pageTable* level2PageTableptrSeg1 = (pageTable*) calloc(1, sizeof(pageTable));
	pageTable* level3PageTableptrSeg1 = (pageTable*) calloc(1, sizeof(pageTable));

	pageTable level1PageTableSeg1 = *level1PageTableptrSeg1;
	pageTable level2PageTableSeg1 = *level2PageTableptrSeg1;
	pageTable level3PageTableSeg1 = *level3PageTableptrSeg1;

	frameOfPageTable* level1PageTableFramesptrSeg1 = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE, sizeof(frameOfPageTable));
	frameOfPageTable* level2PageTableFramesptrSeg1 = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE, sizeof(frameOfPageTable));
	frameOfPageTable* level3PageTableFramesptrSeg1 = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE, sizeof(frameOfPageTable));

	level1PageTableSeg1.frames = level1PageTableFramesptrSeg1;
	level2PageTableSeg1.frames = level2PageTableFramesptrSeg1;
	level3PageTableSeg1.frames = level3PageTableFramesptrSeg1;

	printf("Page tables done\n");
	pageTable* arrayPT[8];
	arrayPT[0] = &level3PageTableSeg0;
	arrayPT[1] = &level3PageTableSeg1;
	initSegTable(&LDT,arrayPT);

	printf("Page table objects declared and frames alloted to them\n");


	for(i=0;i<n;i++)
	{
		scanf("%lx",&addr[i]);
		//printf("%x\n",addr[i]);
	}

	initFrameTable();
	
	initPageTable(&level3PageTableSeg0,&level2PageTableSeg0,&level1PageTableSeg0);
	initPageTable(&level3PageTableSeg1,&level2PageTableSeg1,&level1PageTableSeg1);

	initPCB(&pcbArr[0],&level3PageTableSeg0,&LDT);

	//initPCB(pcbArr[0],&level3PageTableSeg1);

	// Set up done. Start simulation
	// allocateFrame(1,level1PageTable,0,1);
	// allocateFrame(1,level1PageTable,1,1);	//need to enter in page tables too
	//allocateFrame(1,1);


	i = 0;
	unsigned int* pageNo = calloc(1, sizeof(unsigned int));
	unsigned int* level = calloc(1,sizeof(unsigned int));
	pageTable **ptref = malloc(sizeof(pageTable *));

	printf("init done\n");
	while(i < n)
	{
		int26 address;
		address.value = addr[i]>>10;
		pageTable *pt = searchSegmentTable(0,address);
		printf("pagetable fetched %lx\t%lx\n",addr[i],(addr[i]&0xFFFFFFFF));
		if(searchPageTable(pt, ptref, addr[i]&0xFFFFFFFF,pageNo,level) == -1){
			printf("After returing to testMM searchPageTable: level = %d, pageFaultPageNumber = %d\n",*level,*pageNo);
			
			allocateFrame(1,*ptref,*pageNo,*level);
			/*if(*level == 0){
				allocateFrame(1,&level1PageTable, *pageNo, *level);	
			}
			else if(*level == 1){
				allocateFrame(1,&level2PageTable, *pageNo, *level);	
				//printf("\nMain:the present bit =%d\n",level2PageTable.frames[*pageNo/256].entries[*pageNo%256].present);

			}
			else{
				allocateFrame(1,&level3PageTable, *pageNo, *level);
				//printf("\nMain:the present bit =%d\n",level3PageTable.frames[*pageNo/256].entries[*pageNo%256].present);

			}*/
			// We need to retry for this address
			printf("Retrying for same page after page fault\n");
			//printf("------------------------------------\n");

			continue;
		}
		else{
			printf("Search in page table was successful\n");
			i++;		
		}
				//printf("------------------------------------\n");

	}

	free(level1PageTableptrSeg1);
	free(level2PageTableptrSeg1);
	free(level3PageTableptrSeg1);
	free(level1PageTableFramesptrSeg1);
	free(level2PageTableFramesptrSeg1);
	free(level3PageTableFramesptrSeg1);


	free(level1PageTableptrSeg0);
	free(level2PageTableptrSeg0);
	free(level3PageTableptrSeg0);
	free(level1PageTableFramesptrSeg0);
	free(level2PageTableFramesptrSeg0);
	free(level3PageTableFramesptrSeg0);
	free(pageNo);
	free(level);
	return 0;
}