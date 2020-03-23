#include "dataTypes.h"

#define NUM_FRAMES 65536

typdef struct
{
	int22 pageNum;
	int4 segNum;
	int pid;
	int16 LfuCount;
	unsigned int dirtyBit: 1;
	unsigned int emptyBit: 1; // Tells whether a frame has been allocated or not.
	unsigned int considerInLfu: 1;
}
frameTableEntry;


typdef struct 
{
	frameTableEntry entries[NUM_FRAMES];
}
FrameTable;

int findEmptyFrame();
int getLfuFrameNum();
short unsigned getDirtyBitFrameTable(int index);
short unsigned setDirtyBitFrameTable(int index, int value);
short unsigned getEmptyBitFrameTable(int index);
short unsigned setEmptyBitFrameTable(int index, int value);
replacementAlgorithm();
