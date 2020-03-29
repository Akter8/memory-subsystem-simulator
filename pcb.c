#include<stdio.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "segmentTable.h"
// #include "frameTable.h"
#include "pcb.h"

extern PCB pcbArr[1];

int initPCB(PCB *pcbObj, segmentTable* LDTptr){
	// pcbObj->level3PageTablePointer = level3PageTablePointer;
	pcbObj->state = READY;
	pcbObj->LDTPointer = LDTptr;

	initSegTable(LDTptr);

	return 0;
}

int getState(PCB pcbObj){
	return pcbObj.state;
}

int setState(PCB *pcbObj, int state){
	pcbObj->state = state;
	return 0;
}

pageTable* getLevel3PageTablePointer(PCB pcbObj, int segNum){
	return pcbObj.LDTPointer->entries[segNum].level3PageTableptr;
}

int deleteProcess(unsigned int pid){
	PCB pcbObj = pcbArr[pid];
	//Serially free all page tables, then all segment table entries


	//Mark frames of the process as empty in the frame table
	return 0;
}