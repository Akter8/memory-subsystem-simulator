#include<stdio.h>
#include<stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "segmentTable.h"
#include "frameTable.h"
#include "pcb.h"

extern PCB pcbArr[1];
extern FrameTable frameTable;

int initPCB(PCB *pcbObj){
	// pcbObj->level3PageTablePointer = level3PageTablePointer;
	pcbObj->state = READY;
	printf("Calling initSegTable from inside initPCB\n");

	pcbObj->LDTPointer = initSegTable();

	// initSegTable(LDTptr);
	printf("initSegTable returned successfully\n");

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
	segmentTable* LDTptr = pcbObj.LDTPointer;

	pageTable* level3PageTableptr = LDTptr->entries[0].level3PageTableptr;
	pageTable* level2PageTableptr = level3PageTableptr->nextLevelPageTablePointer;
	pageTable* level1PageTableptr = level2PageTableptr->nextLevelPageTablePointer;

	frameOfPageTable* level3PageTableFramesptr = level3PageTableptr->frames;
	frameOfPageTable* level2PageTableFramesptr = level2PageTableptr->frames;
	frameOfPageTable* level1PageTableFramesptr = level1PageTableptr->frames;

	free(level1PageTableFramesptr);
	free(level2PageTableFramesptr);
	free(level3PageTableFramesptr); 

	free(level1PageTableptr);
	free(level2PageTableptr);
	free(level3PageTableptr);


	//free(LDTptr->entries);

	free(LDTptr);

	//Mark frames of the process as empty in the frame table
	for(int i = 0; i < NUM_FRAMES; i++){
		if(frameTable.entries[i].emptyBit == 1 && frameTable.entries[i].pid == pid){
			frameTable.entries[i].emptyBit = 0;
		}
	}
	return 0;
}