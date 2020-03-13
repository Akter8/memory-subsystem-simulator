#include<stdbool.h>
#include"dataTypes.h"

typedef struct 
{
	/*
		data field - 32 bytes
		int data[32]; 
	*/

	unsigned int tag:16;
	unsigned int validInvalidBit:1;
	unsigned int readWriteBit:1;
}dataBlockEntryL1;	


typedef struct 
{
	dataBlockEntryL1 ways[4];
	short unsigned int LRUSquareMatrix[4][4];
}setL1;

//instr and data L1 cache 
typedef struct 
{
	setL1 sets[32];
}L1Cache;


//L2
typedef struct 
{
	/*
		data field - 64 bytes
		int data[64]; 
	*/

	unsigned int tag:14;
	unsigned int validInvalidBit:1;
	unsigned int dirtyBit:1;
	unsigned int readWriteBit:1;
	unsigned int LRUCount:3;
}dataBlockEntryL2;

typedef struct 
{
	dataBlockEntryL2 ways[8];

}setL2;
 
typedef struct 
{
	setL2 sets[64];
}L2Cache;

//L1 Cache
int searchL1Cache(physicalAddr addr, int index); // returns hit/miss, index 0 for inst 1 for data
int updateL1Cache(physicalAddr addr, int index, bool write);
int getLRUL1Cache(int index);
int updateLRUL1Cache(int index, int setIndex); 
int writeL1Cache(int index, int setIndex);


//L2 Cache 
int searchL2Cache(physicalAddr addr); // returns hit/miss, index 0 for inst 1 for data
int updateL2Cache(physicalAddr addr, bool write);
int getLRUL2Cache();
int updateLRUL2Cache(int setIndex); 
int writeL2Cache(int setIndex);
