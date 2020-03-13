// Main memory to be split into 2 parts, one fixed and one in which page replacement will happen
#include "dataTypes.h"

typedef struct
{
	unsigned int frameNumber : 16;	// There are 2**16 frames present
	unsigned int modified : 1;
	unsigned int present : 1;
	unsigned int cachingDisabled : 1;
	unsigned int readWrite : 1;
}pageTableEntry;

typedef struct
{
	pageTableEntry entries[256];
}frameOfPageTable;


typedef struct
{
	// Indexed by page number
	frameOfPageTable *frames;
}pageTable;

int searchPageTable(int16 linearAddress);
int updatePageTableModifiedBit(int index, int value, bool Outer);
int updatePageTablePresentBit(int index, int value, bool Outer);
int deletePageTable(); //To be used when process terminates
int loadFrameOfPageTable(int16 linearAddress); //loads required frame of page table which is currently present in disk
