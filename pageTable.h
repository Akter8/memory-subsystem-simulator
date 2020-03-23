// Main memory to be split into 2 parts, one fixed and one in which page replacement will happen
// #include "dataTypes.h"
#include "configuration.h"
#define NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE 256
#define NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE 256
#define NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE 256
#define NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE 256
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
	pageTableEntry entries[NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE];
}frameOfPageTable;


typedef struct
{
	// Indexed by page number
	frameOfPageTable *frames;
}pageTable;

int initPageTable();
int searchPageTable(unsigned int linearAddr, unsigned int* pageFaultPageNumber);
int updatePageTableModifiedBit(unsigned int index, int value, int level);
int updatePageTablePresentBit(int index, int value, int level);
int deallocateProcessPages(); //To be used when process terminates

