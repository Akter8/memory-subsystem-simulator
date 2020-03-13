// Main memory to be split into 2 parts, one fixed and one in which page replacement will happen
#include "dataTypes.h"

typedef struct
{
	unsigned int baseAddress : 16;	// There are 2**16 frames present
	unsigned int limit : 26
	unsigned int present : 1;
	unsigned int readWrite : 1;
}segmentTableEntry;

typedef struct
{
	// Indexed by page number
	segmentTableEntry entries[4];   //GDT,LDT each contain 4 segments max each
}segmentTable;

int searchSegmentTable(int22 virtualAddress);
int updatePageTablePresentBit(int index, int value);	//value = 0 or 1
int deleteSegmentTable(); //To be used when process terminates

