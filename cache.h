#include <stdbool.h>

//#include "dataTypes.h"
//#include "configuration.h"


typedef struct 
{
	/*
		data field - 32 bytes
		int data[32]; 
	*/

	unsigned int tag: 16;
	unsigned int validInvalidBit: 1;
	unsigned int readWriteBit: 1;
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


// Initialises the cache, valid-invalid bit set to 0
void initL1Cache(); 

// Prints the cache in its current state.
void printL1Cache(int setIndex, bool dataCache); 

// Returns the LRU way index for a particular set
int getLruIndexL1Cache(int setIndex, bool dataCache); 

// Updates the LRU of given setIndex and wayIndex
void updateLruL1Cache(int setIndex, int wayIndex, bool dataCache); 

// Searches for matching tag in the set, returns data if hit, else -1, index 0 for instruction cache 1 for dataCache. Also updates the LRU if tag matches
int searchL1Cache(int setIndex, int tag, bool dataCache); 

// Returns the first invalid way's index if present, else returns -1.
int getFirstInvalidWayL1Cache(int setIndex, bool dataCache); 

// Finds a way in that index of the cache to store the new data, replaces that block with the new block
int updateL1Cache(int setIndex, int tag, bool write, int data, bool dataCache); 

// Writes data onto that cache entry, if tag matches, if no tag matches then returs a TAG_NO_MATCH error, if the instruction is read returns ERROR_CANNOT_WRITE_IN_INST_CACHE. The function also updates the LRU of the corresponding set if tag matches.
int writeL1Cache(int setIndex, int tag, int data, bool dataCache); 


// L2 Cache
void initL2Cache(); // Initialises the cache.
void printL2Cache(int setIndex); // Prints the cache in its current state.
int getLruIndexL2Cache(int setIndex); // Returns the way index of the LRU way.
void updateLruL2Cache(int setIndex, int wayIndex); // Updates the LRU counts.
int searchL2Cache(int setIndex, int tag); // Returns data if hit, else -1.
int getFirstInvalidWayL2Cache(int setIndex); // Returns the first invalid way's index, else -1.
int updateL2Cache(int setIndex, int tag, bool write, int data); // Finds a way in that index of the cache to store the new data.
int writeL2Cache(int setIndex, int tag, int data); // Writes data onto that cache entry.
