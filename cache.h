#include <stdbool.h>

#include "configuration.h"

// Since L1 cache is a split cache, we need to make two parts of L1.
// Instead of having a cache of 8K split into data and instr parts,
// we have created two instances of a cache of 4K size.
typedef struct 
{
	/*
		data field - 32 bytes
		int data[32]; 
	*/

	/* 4KB with 32B block and 29b linear address size will give us
	 * 19 bits for tag,
	 * 5 bits for set index,
	 * 5 bits for offset.
	 */

	// Though the linear address is 26b, we have added 3b for future updgradation.

	// Two instances of the 4KB cache will give us a total size of 8KB.

	unsigned int tag: 16;
	unsigned int validInvalidBit: 1;
	unsigned int readWriteBit: 1;

	// No modified/dirty bit as this cache follows write-through.
}
DataBlockEntryL1;	


typedef struct 
{
	DataBlockEntryL1 ways[NUM_WAYS_IN_L1_CACHE];
	short unsigned int lruSquareMatrix[NUM_WAYS_IN_L1_CACHE][NUM_WAYS_IN_L1_CACHE];
}
SetL1;

//instr and data L1 cache 
typedef struct 
{
	SetL1 sets[NUM_SETS_IN_L1_CACHE];
}
CacheL1;



//L2
typedef struct 
{
	/*
		data field - 64 bytes
		int data[64]; 
	*/

	/* 32KB with 64B block size and 29b linear address will give us
	 * 17 bits for tag,
	 * 6 bits for set index,
	 * 6 bits for offset.
	 */

	// Though the linear address is 26b, we have added 3b for future updgradation.

	unsigned int tag: 14;
	unsigned int validInvalidBit: 1;
	unsigned int dirtyBit: 1;
	unsigned int readWriteBit: 1;
	unsigned int lruCount: LOG_NUM_WAYS_IN_L2_CACHE;
}
DataBlockEntryL2;

typedef struct 
{
	DataBlockEntryL2 ways[NUM_WAYS_IN_L2_CACHE];
}
SetL2;
 
typedef struct 
{
	SetL2 sets[NUM_SETS_IN_L2_CACHE];
}
CacheL2;




// Functions
// L1 Cache
// bool dataCache corresponds to whether we are accessing the data or instruction cache of L1.
void initL1Cache(); // Initialises the cache.
void printL1Cache(int setIndex, bool dataCache); // Prints the cache in its current state.
int getLruIndexL1Cache(int setIndex, bool dataCache); // Returns the LRU way index for that set
void updateLruL1Cache(int setIndex, int wayIndex, bool dataCache); // Updates the LRU.
int searchL1Cache(int setIndex, int tag, bool dataCache); // returns data if hit, else -1, index 0 for inst 1 for data
int getFirstInvalidWayL1Cache(int setIndex, bool dataCache); // Returns the first invalid way's index, else -1.
int updateL1Cache(int setIndex, int tag, bool write, int data, bool dataCache); // Finds a way in that index of the cache to store the new data.
int writeL1Cache(int setIndex, int tag, int data, int l2CacheIndex, int l2CacheTag, bool dataCache); // Writes data onto that cache entry.


// L2 Cache
void initL2Cache(); // Initialises the cache.
void printL2Cache(int setIndex); // Prints the cache in its current state.
int getLruIndexL2Cache(int setIndex); // Returns the way index of the LRU way.
void updateLruL2Cache(int setIndex, int wayIndex); // Updates the LRU counts.
int searchL2Cache(int setIndex, int tag); // Returns data if hit, else -1.
int getFirstInvalidWayL2Cache(int setIndex); // Returns the first invalid way's index, else -1.
int updateL2Cache(int setIndex, int tag, bool write, int data); // Finds a way in that index of the cache to store the new data.
int writeL2Cache(int setIndex, int tag, int data); // Writes data onto that cache entry.