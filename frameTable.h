// #include "dataTypes.h"

#define NUM_FRAMES 65536



typedef struct
{
	int26 pageNum;
	int pid;
	int16 LfuCount;
	unsigned int dirtyBit: 1;
	unsigned int emptyBit: 1; // Tells whether a frame has been allocated or not | set if allocated
	unsigned int considerInLfu: 1;
}
frameTableEntry;


typedef struct 
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
int allocateFrame(int pid,int pageNum);
int invalidateFrame(int frameNo);
int initFrameTable();
int frameAgeing();
int updateLfuCount(int frameNo);
