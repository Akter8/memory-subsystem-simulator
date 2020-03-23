#include "dataTypes.h"

#define RUNNING 0
#define READY 1
#define WAIT 2
#define TERMINATED 3

typdef struct
{
	int26 LDTBaseAddress;
	int26 PageTableBaseAddress;
	int state;
	FILE *LinearAddrInputFile;
	FILE *SegNumAddrFile;
}
PCB;

int getState(PCB pcbObj);
int setState(PCB pcbObj, int state);
int26 getPageTableBaseAddress(PCB pcbObj);
int26 getLDTBaseAddress(PCB pcbObj);
