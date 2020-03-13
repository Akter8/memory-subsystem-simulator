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
	FILE *inputFile;
}
PCB;

int getState(int index);
int setState(int index, int state);
int26 getPageTableBaseAddress(int index);
int26 getLDTBaseAddress(int index);