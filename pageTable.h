#include "configuration.h"
#define NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE 256
#define NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE 1
#define NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE 64
#define NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE 16384
typedef struct
{
	unsigned int frameNumber : 16;		// There are 2**16 frames present
	unsigned int modified : 1;			//dirty bit
	unsigned int present : 1;			//is page currenttly present in MM
	unsigned int cachingDisabled : 1;	//for page table pages
	unsigned int readWrite : 1;			//R/W protection
	void* frameOfNextLevel;				//void* to avoid compilation error
}pageTableEntry;

typedef struct
{
	pageTableEntry entries[NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE];	//stores all entries of a single frame of page table
}frameOfPageTable;


struct pageTable
{
	// Indexed by page number
	frameOfPageTable *frames;
	struct pageTable* nextLevelPageTablePointer;		//Pointer to next level page table in multilevel paging
};
typedef struct pageTable pageTable;

//Initilizes page table
pageTable* initPageTable(int readWrite);

//Search page table for given page number
int searchPageTable(pageTable* level3PageTable,pageTable **ptref,unsigned int linearAddr,unsigned int readWrite, unsigned int* pageFaultPageNumber,unsigned int* level);

// Set or reset modified bit of page table entry
int updatePageTableModifiedBit(pageTable* pageTableptr,unsigned int index, int value);

// Set or reset present bit of page table entry
int updatePageTablePresentBit(pageTable *pT, unsigned int index, int value);

// Get page table of required level corresponding to a segment of a process
pageTable* getPageTableFromPid(unsigned int pid,unsigned int segNum,unsigned int level);

// Assign a physical frame number to a page
int setFrameNo(pageTable *pT, unsigned int index, int value);

// Find first code page and data page accessed by process
int findFirst2Pages(char *linearAddrInp,char *segNumInp,int *pageCode,int *pageData);

// Allocate memory for first data page and first code page to be accessed by process before it starts execution
int prepaging(int pid,char *LinearAddrInputFile,char *SegNumAddrFile);
