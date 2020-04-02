// Main memory to be split into 2 parts, one fixed and one in which page replacement will happen
//#include "dataTypes.h"

typedef struct
{
	unsigned int baseAddress : 19;	// There are 2**16 frames present
	unsigned int limit : 32;         //Segment limit is based upon our design for each process, for now we keep it 32 bits
	unsigned int present : 1;
	unsigned int readWrite : 1;
    pageTable* level3PageTableptr;     // Pointer to level3PageTable
}segmentTableEntry;

typedef struct
{
	// Indexed by page number
	segmentTableEntry entries[8];   //GDT,LDT each contain 4 segments max each
    int GDT;                        // 0 means segmentTable is LDT, 1 means it is GDT
}segmentTable;

typedef struct
{
    segmentTable segmentTableObj;
    //int29 SegTableBaseAddress;
}segmentTableInfo;

// segmentTableInfo* initLDTable();
// segmentTableInfo* initGDTable();
// pageTable* searchSegmentTable(int pid, int4 segNum);			
// int deleteSegmentTable(segmentTable* segTableObj); //To be used when process terminates
// void createGDTsegment(int index, int limit);
// //int updatePageTablePresentBit(segmentTable* segTableObj, int index, int value);	//value = 0 or 1


pageTable* searchSegmentTable(int pid, int4 segNum);			
int updateSegmentTablePresentBit(segmentTableEntry* segTableEntry, int index, int value);
segmentTable* initSegTable();
