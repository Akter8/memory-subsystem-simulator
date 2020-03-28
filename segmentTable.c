#include<stdio.h>
#include "dataTypes.h"
#include "pageTable.h"		
#include "segmentTable.h"
#include "pcb.h"
extern PCB pcbArr[1];
extern segmentTable GDT;
pageTable* searchSegmentTable(int pid, int26 virtualAddress)
{
	unsigned int segNo = virtualAddress.value>>23;
	unsigned int localGlobal = (virtualAddress.value>>22)&1;

	if(localGlobal==0)
	{
		//Global Descriptor table
		if(GDT.entries[segNo].present==1)
			return pcbArr[pid].LDTPointer->entries[segNo].level3PageTable;	

	}
	else
	{
		//Local Descriptor table
		if(pcbArr[pid].LDTPointer->entries[segNo].present==1)
			return pcbArr[pid].LDTPointer->entries[segNo].level3PageTable;	
	}

	return 0;
}				
int updateSegmentTablePresentBit(segmentTableEntry* segTableEntry, int index, int value){

		segTableEntry->present = value;
		return 1;

}	//value = 0 or 1
int deleteSegmentTable(){

}

int initSegTable(segmentTable *segtable,pageTable *array[8])
{
	for(int i=0;i<8;i++)
		segtable->entries[i].level3PageTable  = array[i];

	return 0;
}