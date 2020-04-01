#include<stdio.h>
#include<stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "segmentTable.h"
#include "frameTable.h"
#include "pcb.h"

extern PCB pcbArr[1];
extern FrameTable frameTable;


/*
Initializes ADT of PCB and calls method
to initialize LDT of the process
*/
int initPCB(PCB *pcbObj){
	pcbObj->state = READY;

	pcbObj->LDTPointer = initSegTable();

	return 0;
}




/*
Takes ADT of PCB as input and
returns the process state 
*/
int getState(PCB pcbObj){
	return pcbObj.state;
}




/*
Takes ADT of PCB and state as input and
sets the process state 
*/
int setState(PCB *pcbObj, int state){
	pcbObj->state = state;
	return 0;
}




/*
Takes ADT of PCB and sement number as input
and returns pointer to level 3 page table of that segment
*/
pageTable* getLevel3PageTablePointer(PCB pcbObj, int segNum){
	return pcbObj.LDTPointer->entries[segNum].level3PageTableptr;
}




/*
Frees dynamic memory allocated for LDT and page tables
of a process and marks the frames occupied by the process' pages as empty
*/
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


	// free(LDTptr->entries);
	// Entries are not being freed currently. This is a bug that is causing a memory leak

	free(LDTptr);

	//Mark frames of the process as empty in the frame table
	for(int i = 0; i < NUM_FRAMES; i++){
		if(frameTable.entries[i].emptyBit == 1 && frameTable.entries[i].pid == pid){
				invalidateFrame(i);
		}
	
	}
	return 0;
}
