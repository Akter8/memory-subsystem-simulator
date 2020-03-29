#include<stdio.h>
#include<stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"		
#include "segmentTable.h"
#include "pcb.h"
extern PCB pcbArr[1];
extern segmentTable* GDTptr;

pageTable* searchSegmentTable(int pid, int26 virtualAddress)
{
	unsigned int segNo = virtualAddress.value>>23;
	unsigned int localGlobal = (virtualAddress.value>>22)&1;
	if(localGlobal==1)
	{
		//Global Descriptor table
		if(GDTptr->entries[segNo].present==1)
			return pcbArr[pid].LDTPointer->entries[segNo].level3PageTableptr;	

	}
	else
	{
		//Local Descriptor table
		if(pcbArr[pid].LDTPointer->entries[segNo].present==1){
			return pcbArr[pid].LDTPointer->entries[segNo].level3PageTableptr;	
		}
	}

	return 0;
}				
int updateSegmentTablePresentBit(segmentTableEntry* segTableEntry, int index, int value){

		segTableEntry->present = value;
		return 1;

}	//value = 0 or 1
int deleteSegmentTable(){

}

int initSegTable(segmentTable *segTableptr)
{
	// pageTable* pageTableArrayptr = calloc(8,sizeof(pageTable*));

	segTableptr = calloc(1,sizeof(segmentTable));
	segmentTableEntry* entries = calloc(8,sizeof(segmentTableEntry));


	for(int i = 0; i < 8; i++){
		segTableptr->entries[i] = entries[i];
	}

	for(int i=0;i<8;i++)
	{
		segTableptr->entries[i].present=1;
		segTableptr->entries[i].level3PageTableptr  = NULL;
	}

	//Now initialize page table of single segment
	pageTable *level1PageTableptr, *level2PageTableptr, *level3PageTableptr;
	frameOfPageTable *level1PageTableFramesptr, *level2PageTableFramesptr, *level3PageTableFramesptr;

	initPageTable(level3PageTableptr,level2PageTableptr,level1PageTableptr,level3PageTableFramesptr,
		level2PageTableFramesptr,level1PageTableFramesptr);

	segTableptr->entries[0].level3PageTableptr = level3PageTableptr;



	return 0;
}