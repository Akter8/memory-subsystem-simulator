// Main memory to be split into 2 parts, one fixed and one in which page replacement will happen
//#include "dataTypes.h"

typedef struct
{
	unsigned int baseAddress : 16;	// There are 2**16 frames present
	unsigned int limit : 26;
	unsigned int present : 1;
	unsigned int readWrite : 1;
	pageTable* level3PageTable;

}segmentTableEntry;

typedef struct
{
	// Indexed by page number
	segmentTableEntry entries[8];   //GDT,LDT each contain 4 segments max each
}segmentTable;

pageTable* searchSegmentTable(int pid, int26 virtualAddress);			//Check whether to search in LDT or GDT
int updateSegmentTablePresentBit(segmentTableEntry* segTableEntry, int index, int value);
int deleteSegmentTable(); //To be used when process terminates
int initSegTable(segmentTable *segtable,pageTable *array[8]);

