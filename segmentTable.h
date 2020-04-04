typedef struct
{
	//unsigned int baseAddress : 19;			// There are 2**16 frames present
	unsigned int limit : 32;         			// Segment size
	unsigned int present : 1;					// Whether segment is present in MM
	unsigned int readWrite : 1;					// Code segment(read) or data segment(write)
    pageTable* level3PageTableptr;				// Pointer to level3PageTable of the segment
}segmentTableEntry;

typedef struct
{
	// Indexed by page number
	segmentTableEntry entries[8];   			//GDT,LDT each contain 8 segments max each
    int GDT;                        			// 0 means segmentTable is LDT, 1 means it is GDT
}segmentTable;

typedef struct
{
    segmentTable *segmentTableObj;				
   	unsigned int SegTableBaseAddress;			// segment base address in MM
}segmentTableInfo;

// Initialize GDT
segmentTableInfo* initGDTable();

// Initialize a new entry in GDT
void createGDTsegment(int index, int limit);

// Initialize LDT
segmentTableInfo* initLDTable(int limit);

// Return pointer to level 3 page table of a particular segment of a process
pageTable* searchSegmentTable(int pid, int4 segNum);			

// Set or reset present bit of segment table entry
int updateSegmentTablePresentBit(segmentTableEntry* segTableEntry, int index, int value);

