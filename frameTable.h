// #include "dataTypes.h"

#define NUM_FRAMES 65536
#define NUM_NON_REPLACABLE_FRAMES 10


typedef struct
{
	int26 pageNum;
	int pid;
	int16 LfuCount;
	unsigned int dirtyBit: 1;
	unsigned int emptyBit: 1; // Tells whether a frame has been allocated or not | set if allocated
	unsigned int considerInLfu: 1;
	int level : 2;			  // which level of page table would contain an entry corresponding to pageNum
	unsigned int segNum : 4;
}
frameTableEntry;


typedef struct 
{
	frameTableEntry entries[NUM_FRAMES];
}
FrameTable;

int getReplacableEmptyFrame();
int getNonReplaceableFrame();
int getLfuFrameNum();
short unsigned getDirtyBitFrameTable(int index);
short unsigned setDirtyBitFrameTable(int index, int value);
short unsigned getEmptyBitFrameTable(int index);
short unsigned setEmptyBitFrameTable(int index, int value);
int allocateFrame(int pid,int segno,pageTable *pT,int pageNum,int level);
int invalidateFrame(int frameNo);
int initFrameTable();
int frameAgeing();
int updateLfuCount(int frameNo);
