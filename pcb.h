#include "dataTypes.h"

#define RUNNING 0
#define READY 1
#define WAIT 2
#define TERMINATED 3

typdef struct
{
	int19 LDTBaseAddress;
    segmentTable* LDTableObj;
	int state;
	FILE *LinearAddrInputFile;
	FILE *SegNumInputFile;
    int4 GDTindex;
    int19 GDTBaseAddress;
    long long swapStartTime;    //Time at which swapping starts when page fault occurs
    long long runTime;          //Amount of time the process has spent executing in CPU
}
PCB;


int getState(PCB pcbObj);
int setState(PCB pcbObj, int state);
int19 getLDTBaseAddress(PCB pcbObj);
void initPCB(PCB pcbObj, char* LinearAddrInputFileName, char* segInputFileName);
