/*
 * Author: Akhil
 * Date: 13/3/20
 *
 * Contains the ADT of the TLB.
 */

//#include "dataTypes.h"

//#include "configuration.h"

typedef struct 
{
	unsigned int validInvalidBit: 1;
	unsigned int pageNum: PAGE_NUM_ADDR_SIZE;
	unsigned int frameNum: FRAME_NUM_ADDR_SIZE;
	unsigned int lruCount: 4;
}
TLBL1Entry;

typedef struct 
{
	unsigned int validInvalidBit: 1;
	unsigned int pageNum: PAGE_NUM_ADDR_SIZE;
	unsigned int frameNum: FRAME_NUM_ADDR_SIZE;
	unsigned int lruCount: 5;
}
TLBL2Entry;

typedef struct
{
	TLBL1Entry entries[NUM_ENTRIES_IN_L1_TLB];
}
TLBL1;


typedef struct 
{
	TLBL2Entry entries[NUM_ENTRIES_IN_L2_TLB];
}
TLBL2;




// Functions


// L1

// Invalidates all entries.
void TLBL1Flush(); 

// Prints the current state of the entire TLB table.
void TLBL1Print(); 

// Searches for particular page number's in TLB. If hit the physical address is returned else returns -1, second paramenter - error = ERROR_PAGE_NUM_NOT_FOUND. Also updates the corresponding lru
unsigned int TLBL1Search(unsigned int pageNum, unsigned int *error); 

// Replaces a block in tlb with the new block - with new pageNum and frameNum mapping. Returns the index where the new entry was put.
int TLBL1Update(unsigned int pageNum, unsigned int frameNum); 

// Mimics the entry of index being used and hence updates the LRU corresponding to the index.
int TLBL1UpdateLru(int index); 

// Returns the index of the lru entry in the TLB.
int TLBL1GetLruIndex(); 

// Returns the first invalid entry in the TLB.
int TLBL1GetFirstInvalidEntry(); 




// L2

// Invalidates all entries.
void TLBL2Flush(); 

// Prints the current state of the entire TLB table.
void TLBL2Print();

// Searches for particular page number's in TLB. If hit the physical address is returned else returns -1, second paramenter - error = ERROR_PAGE_NUM_NOT_FOUND. Also updates the corresponding lru
unsigned int TLBL2Search(unsigned int pageNum, unsigned int *error); 

// Replaces a block in tlb with the new block - with new pageNum and frameNum mapping. Returns the index where the new entry was put.
int TLBL2Update (unsigned int pageNum, unsigned int frameNum); 

// Mimics the entry of index being used and hence updates the LRU corresponding to the index.
int TLBL2UpdateLru(int index); 

// Returns the index of the lru entry in the TLB.
int TLBL2GetLruIndex(); 

// Returns the first invalid entry in the TLB.
int TLBL2GetFirstInvalidEntry(); 
