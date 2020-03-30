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
	unsigned int segNo = (virtualAddress.value>>22)&0x7;
	unsigned int localGlobal = (virtualAddress.value>>25);

	//printf("segment number=%d\n",segNo);
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
int deleteSegmentTable(segmentTable* segtable){

	//delete all the page tables allocate.
	// for(i=0;i<8;i++)
	// {
	// 	if(segtable->entries[i].present==1)
	// 	{
	// 		deallocateProcessPages(segtable->entries[i].level3PageTableptr);
	// 	}
	// }


}
int initSegment()	//Function to initialise a segment in the segment table
{
	//initialise page tables and present bit of the correspondinf segment

}
segmentTable* initSegTable()
{
	// pageTable* pageTableArrayptr = calloc(8,sizeof(pageTable*));
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

	//Now initialize page table of single segment
	//pageTable *level1PageTableptr, *level2PageTableptr, *level3PageTableptr;
	//frameOfPageTable *level1PageTableFramesptr, *level2PageTableFramesptr, *level3PageTableFramesptr;

	//initPageTable(level3PageTableptr,level2PageTableptr,level1PageTableptr,level3PageTableFramesptr,
	//	level2PageTableFramesptr,level1PageTableFramesptr);

	//segTableptr->entries[0].level3PageTableptr = level3PageTableptr;
	segTableptr->entries[0].level3PageTableptr = initPageTable();
	segTableptr->entries[1].level3PageTableptr = initPageTable();

	return segTableptr;

}