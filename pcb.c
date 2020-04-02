#include "pcb.h"
#include "utilityFunctions.c"

int getState(PCB pcbObj)
{
   return pcbObj.state; 
}

//Sets the state of the process - READY/RUNNING/WAIT/TERMINATED
int setState(PCB pcbObj, int state)
{
    pcbObj.state = state;
    return pcbObj.state;
}

//returns the BaseAddress of Local Descriptor Table for the particular process
int29 getLDTBaseAddress(PCB pcbObj)
{
    return pcbObj.LDTBaseAddress;
}

//Returns pid of the process
int getpid(pcbObj)
{
    return pcbObj.pid;
}

//Initializes the PCB for each process
void initPCB(int pid, char* LinearAddrInputFileName, char* segInputFileName)
{
    //get NonReplacable for PCB for the process
    int19 frameNum = getNonReplacableFrame();


    //Initially all processes join the Ready queue
    setState(PCB[pid], READY);


    PCB[pid].swapStartTime = -1;        //denotes not gone for swapping

    //Opening input file for the particular process
    PCB[pid].LinearAddrInputFile = fopen(LinearAddrinputFileName, "r");
    //Check if File open correctly
    fileNotNull(pcbObj.LinearAddrInputFile, LinearAddrInputFileName);


    PCB[pid].SegNumInputFile = fopen(segInputFileName);
    //Check if File open correctly
    fileNotNull(PCB[pid].segInputFile, segInputFileName);


    //Here we'll find limit for each segment of the process
    //


    //Initialize a new Local Descriptor Segment Table for the process
    SegmentTableInfo* segTableInfoObj = initLDTable(limit);
    PCB[pid].LDTBaseAddress = segTableInfoObj->LDTBaseAddress;
    PCB[pid].segmentTableObj = segTableInfoObj->segmentTableObj;


    //To find GDTindex (i.e. free entry ) in the Global Descriptor Table
    for(int i = 0; i < 8; ++i)
    {
        if(GDT->entries[i].present == 0)
        {
            GDTindex = i;
            createGDT(i, limit[i]);
            break;
        }
    }

    free(SegmentTableInfo);
}

