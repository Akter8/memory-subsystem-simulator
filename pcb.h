// #include "dataTypes.h"

#define RUNNING 0
#define READY 1
#define WAIT 2
#define TERMINATED 3

typedef struct
{
	// int26 LDTBaseAddress;
	// int26 PageTableBaseAddress;
	segmentTable* LDTPointer;
	unsigned int segNum: 4;		
	int state;
	// FILE *LinearAddrInputFile;
	// FILE *SegNumAddrFile;
}PCB;

int initPCB(PCB* pcbObj, segmentTable* segtable);
int getState(PCB pcbObj);
int setState(PCB* pcbObj, int state);
pageTable* getLevel3PageTablePointer(PCB pcbObj,int segNum);
int26 getLDTBaseAddress(PCB pcbObj);
int deleteProcess(unsigned int pid);
