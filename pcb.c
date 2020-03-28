#include<stdio.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "segmentTable.h"
// #include "frameTable.h"
#include "pcb.h"

int initPCB(PCB *pcbObj, pageTable* level3PageTablePointer,segmentTable* segtable){
	pcbObj->level3PageTablePointer = level3PageTablePointer;
	pcbObj->state = READY;
	pcbObj->LDTPointer = segtable;
	return 0;
}

int getState(PCB pcbObj){
	return pcbObj.state;
}

int setState(PCB *pcbObj, int state){
	pcbObj->state = state;
	return 0;
}

pageTable* getLevel3PageTablePointer(PCB pcbObj){
	return pcbObj.level3PageTablePointer;
}
