#include<stdio.h>
#include<stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "segmentTable.h"
#include "frameTable.h"
#include "pcb.h"
#include "utility.h"

extern PCB pcbArr[30];
extern FrameTable frameTable;
extern segmentTableInfo* GDTptr;

/*
Initializes ADT of PCB and calls method
to initialize LDT of the process
*/
// int initPCB(PCB *pcbObj,char *LinearAddrInputFileName,char *SegAddrInputFileName){
//     pcbObj->state = READY;
//     pcbObj->LinearAddrInputFile = fopen(LinearAddrInputFileName,"r");
//     pcbObj->SegNumAddrFile = fopen(SegAddrInputFileName,"r");

//     pcbObj->LDTPointer = initSegTable();

//     return 0;
// }




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



//returns the BaseAddress of Local Descriptor Table for the particular process
// int29 getLDTBaseAddress(PCB pcbObj)
// {
//     return pcbObj.LDTBaseAddress;
// }

//Returns pid of the process
int getpid(PCB pcbObj)
{
    return pcbObj.pid;
}

void findSegmentLimits(int* gdtSeg_limit, int* ldtSeg_limit, FILE* LinearAddrInputFile, FILE* SegNumAddrFile)
{
    int addr;
    while(fscanf(LinearAddrInputFile, "%x", &addr) != EOF)
    {
        int segNum; char write;
        fscanf(SegNumAddrFile, "%d %c", &segNum, &write);
        if(segNum == 8)
        {
            if(addr > *gdtSeg_limit)
            {
                *gdtSeg_limit = addr;
            }
        }
        else if(segNum == 0)
        {
            if(addr > *ldtSeg_limit)
            {
                *ldtSeg_limit = addr;
            }
        }
    }
}

//Initializes the PCB for each process
void initPCB(int pid, char* LinearAddrInputFileName, char* segInputFileName)
{
    //get NonReplacable for PCB for the process
    int frameNum = getNonReplaceableFrame();


    //Initially all processes join the Ready queue
    setState(&pcbArr[pid], READY);


    //PCB[pid].swapStartTime = -1;        //denotes not gone for swapping

    //Opening input file for the particular process
    pcbArr[pid].LinearAddrInputFile = fopen(LinearAddrInputFileName,"r");
    //Check if File open correctly
  //  fileNotNull(pcbArr[pid].LinearAddrInputFile, LinearAddrInputFileName);


    pcbArr[pid].SegNumAddrFile = fopen(segInputFileName,"r");
    //Check if File open correctly
    //fileNotNull(pcbArr[pid].segInputFile, segInputFileName);


    //Here we'll find limit for each segment of the process
    //

    int gdtSegLimit = 0; int ldtSegLimit = 0;
    findSegmentLimits(&gdtSegLimit, &ldtSegLimit, pcbArr[pid].LinearAddrInputFile, pcbArr[pid].SegNumAddrFile);
    gdtSegLimit = (4*gdtSegLimit < 0xFFFFFFFF) ? 4*gdtSegLimit : 0xFFFFFFFF;
    ldtSegLimit = (4*ldtSegLimit < 0xFFFFFFFF) ? 4*ldtSegLimit : 0xFFFFFFFF;

    fclose(pcbArr[pid].LinearAddrInputFile);
    fclose(pcbArr[pid].SegNumAddrFile);

    pcbArr[pid].LinearAddrInputFile = fopen(LinearAddrInputFileName,"r");
    pcbArr[pid].SegNumAddrFile = fopen(segInputFileName,"r");

    //Initialize a new Local Descriptor Segment Table for the process
   // SegmentTableInfo* segTableInfoObj = initLDTable(limit);
  //  PCB[pid].LDTBaseAddress = segTableInfoObj->LDTBaseAddress;
    pcbArr[pid].LDTPointer = initLDTable(ldtSegLimit);

    //To find GDTindex (i.e. free entry ) in the Global Descriptor Table
    for(int i = 0; i < 8; ++i)
    {
        if(GDTptr->segmentTableObj->entries[i].present == 0)
        {
            pcbArr[pid].GDTindex.value = i;
            createGDTsegment(i, gdtSegLimit);
            //GDT.entries[i].present = 1;
            //GDT.entries[i].
            break;
        }
    }

    
    //free(SegmentTableInfo);
}



pageTable* getLevel3PageTablePointer(PCB pcbObj, int segNum){
    return pcbObj.LDTPointer->segmentTableObj->entries[segNum].level3PageTableptr;
}

int deleteProcess(unsigned int pid){
    PCB pcbObj = pcbArr[pid];
    //Serially free all page tables, then all segment table entries
    segmentTable* LDTptr =  pcbObj.LDTPointer->segmentTableObj;

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
    free(pcbObj.LDTPointer);
    //Mark frames of the process as empty in the frame table
    for(int i = 0; i < NUM_FRAMES; i++){
        if(frameTable.entries[i].emptyBit == 1 && frameTable.entries[i].pid == pid){
            invalidateFrame(i);
        }
    }
    return 0;
}

