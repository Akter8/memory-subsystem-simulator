#include <stdbool.h>
#include "dataTypes.h"
#include "configuration.h"


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
	short unsigned int lruSquareMatrix[4][4];
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
	unsigned int lruCount: NUM_WAYS_IN_L2_CACHE;
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
void initL1Cache(); // Initialises the cache.
void printL1Cache(int setIndex, bool dataCache); // Prints the cache in its current state.
// int searchL1Cache(physicalAddr addr, bool dataCache); // returns data if hit else -1, index 0 for inst 1 for data
// int updateL1Cache(physicalAddr addr, int index, bool write);
int getLruL1Cache(int index);
int updateLruL1Cache(int index, int setIndex); 
int writeL1Cache(int index, int setIndex);


// L2 Cache
void initL2Cache(); // Initialises the cache.
void printL2Cache(int setIndex); // Prints the cache in its current state.
int searchL2Cache(int index, int tag); // returns data if hit else -1
int updateL2Cache(int index, int tag, bool write, int data);
int getLruL2Cache(int index); // Returns the way index.
void updateLruL2Cache(int setIndex, int wayIndex); 
int writeL2Cache(int setIndex, int tag, int data);