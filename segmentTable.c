#include<stdio.h>
#include<stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"		
#include "segmentTable.h"
#include "pcb.h"

extern PCB pcbArr[1];
extern segmentTable* GDTptr;




/*
Takes pid virtual address produced by processor as input 
and returns pointer to level 3 page table of required segment
*/
pageTable* searchSegmentTable(int pid, int26 virtualAddress)
{
	unsigned int segNo = (virtualAddress.value>>22)&0x7;
	unsigned int localGlobal = (virtualAddress.value>>25);

	if(localGlobal==1)
	{
		//Global Descriptor table
		printf("The address is in a global segment\n");
		if(GDTptr->entries[segNo].present==1)
			return pcbArr[pid].LDTPointer->entries[segNo].level3PageTableptr;	

	}
	else
	{
		//Local Descriptor table
		printf("The address is in a local segment\n");
		if(pcbArr[pid].LDTPointer->entries[segNo].present==1){
			return pcbArr[pid].LDTPointer->entries[segNo].level3PageTableptr;	
		}
	}

	return 0;
}




/*
Takes segment table entry ADT and value as input and
updates present bit of that entry to value 
*/				
int updateSegmentTablePresentBit(segmentTableEntry* segTableEntry, int index, int value){

		segTableEntry->present = value;
		return 1;

}	//value = 0 or 1




/*
Initializes ADTs for segment table and calls method that initializes page table of one segment 
*/
segmentTable* initSegTable()
{
	segmentTable *segTableptr;
	segTableptr =(segmentTable *) calloc(1,sizeof(segmentTable));
	segmentTableEntry* entries = calloc(8,sizeof(segmentTableEntry));


	for(int i = 0; i < 8; i++){
		segTableptr->entries[i] = entries[i];
	}

	for(int i=0;i<8;i++)
	{
		segTableptr->entries[i].present=1;
		segTableptr->entries[i].level3PageTableptr  = NULL;
	}

	segTableptr->entries[0].level3PageTableptr = initPageTable();

	return segTableptr;

}