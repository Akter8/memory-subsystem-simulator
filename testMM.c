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
	long addr[100];
	int n=20,i;
	printf("Enter input\n");

	for(i=0;i<n;i++)
	{
		scanf("%lx",&addr[i]);
	}

	printf("Initializing frame table\n");
	initFrameTable();
	printf("Initialized frame table\n");

	printf("Initializing GDT\n");
	GDTptr=initSegTable();
	printf("Initilized GDT\n");

	
	printf("Initializing PCB\n");
	initPCB(&pcbArr[0]);
	printf("Initialized PCB\n");

	// Set up done. Start simulation
	// allocateFrame(1,level1PageTable,0,1);
	// allocateFrame(1,level1PageTable,1,1);	//need to enter in page tables too
	//allocateFrame(1,1);

	i = 0;
	unsigned int* pageNo = calloc(1, sizeof(unsigned int));
	unsigned int* level = calloc(1,sizeof(unsigned int));
	pageTable** ptrToPageFaultPageTable = malloc(sizeof(pageTable *));
	pageTable* pagetable;
	int26 address;
	while(i < n)
	{
		address.value = addr[i]>>10;
		pagetable = searchSegmentTable(0, address);
		printf("Calling searchPageTable\n");
		//if(searchPageTable(pcbArr[0].LDTPointer->entries[0].level3PageTableptr, ptrToPageFaultPageTable, addr[i]&0xFFFFFFFF,pageNo,level) == -1){
		if(searchPageTable(pagetable, ptrToPageFaultPageTable, addr[i]&0xFFFFFFFF,pageNo,level) == -1){

			printf("After returing to testMM searchPageTable: level = %d, pageFaultPageNumber = %d\n",*level,*pageNo);
		
			allocateFrame(0,addr[i]>>32,*ptrToPageFaultPageTable, *pageNo, *level);	
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

	deleteProcess(0);
	free(pageNo);
	free(level);
	return 0;
}