/*
 * Author: Akhil
 * Date: 13/3/20
 *
 * Contains the ADT of the TLB.
 */

#include "dataTypes.h"

#include "configuration.h"

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
void TLBL1Flush(); // Invalidates all entries.
void TLBL1Print(); // Prints the current state of the TLB.
unsigned int TLBL1Search(unsigned int pageNum, unsigned int *error); // Second parameter for error.
int TLBL1Update(unsigned int pageNum, unsigned int frameNum); // Returns the index where the new entry was put.
int TLBL1UpdateLru(int index); // Mimics the entry of index being used and hence updates the LRU.
int TLBL1GetLruIndex(); // Returns the index of the entry in the array.
int TLBL1GetFirstInvalidEntry(); // Returns the first invalid entry in the TLB.


// L2
void TLBL2Flush(); // Invalidates all entries.
void TLBL2Print(); // Prints the current state of the TLB.
unsigned int TLBL2Search(unsigned int pageNum, unsigned int *error); // Second paramenter for error.
int TLBL2Update (unsigned int pageNum, unsigned int frameNum); // Returns the index where the new entry was put.
int TLBL2UpdateLru(int index); // Mimics the entry of index being used and hence updates the LRU.
int TLBL2GetLruIndex(); // Returns the index of the entry in the array.
int TLBL2GetFirstInvalidEntry(); // Returns the first invalid entry in the TLB.