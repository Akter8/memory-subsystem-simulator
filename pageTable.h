// Main memory to be split into 2 parts, one fixed and one in which page replacement will happen
// #include "dataTypes.h"
#include "configuration.h"
#define NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE 256
#define NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE 2048
#define NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE 8
#define NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE 1
typedef struct
{
	unsigned int frameNumber : 16;	// There are 2**16 frames present
	unsigned int modified : 1;
	unsigned int present : 1;
	unsigned int cachingDisabled : 1;
	unsigned int readWrite : 1;
	void* frameOfNextLevel;			//void* to avoid compilation error
}pageTableEntry;

typedef struct
{
	pageTableEntry entries[NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE];
}frameOfPageTable;


struct pageTable
{
	// Indexed by page number
	frameOfPageTable *frames;
	struct pageTable* nextLevelPageTablePointer;
};
typedef struct pageTable pageTable;

int initPageTable(pageTable level3PageTable,pageTable level2PageTable, pageTable level1PageTable);
int searchPageTable(pageTable level3PageTable,pageTable level2PageTable, pageTable level1PageTable,unsigned int linearAddr, unsigned int* pageFaultPageNumber,unsigned int *level);
int updatePageTableModifiedBit(pageTable pT,unsigned int index, int value);
int updatePageTablePresentBit(pageTable pT, unsigned int index, int value);
int deallocateProcessPages(pageTable level3PageTable,pageTable level2PageTable, pageTable level1PageTable); //To be used when process terminates
pageTable getPageTableFromPid(unsigned int pid,unsigned int segNum,unsigned int level);

