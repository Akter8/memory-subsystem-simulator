// #include "dataTypes.h"

#define NUM_FRAMES 65536
#define NUM_NON_REPLACABLE_FRAMES 1024


typedef struct
{
	int26 pageNum; 	//Page number 
	int pid; 		
	int16 LfuCount;	//LFU count of the frame
	unsigned int lock: 1;	//lock bit used when frame is allocated
	unsigned int dirtyBit: 1;	//dirty bit
	unsigned int emptyBit: 1; // Tells whether a frame has been allocated or not | set if allocated
	unsigned int considerInLfu: 1; //Bit that indicates whether frame of MM belongs to replaceable of non-replaceable part 
	int level ;			  // which level of page table would contain an entry corresponding to pageNum
	unsigned int segNum : 4;	// segment number to which page belongs 
}
frameTableEntry;


typedef struct 
{
	frameTableEntry entries[NUM_FRAMES];	// Frame Table is array of frameTableEntry
}
FrameTable;


// Returns a replacable frame for allocation or -1 if no frame is available
int getReplacableEmptyFrame();

//Returns an irreplacable frame for allocation or -1 if no frame is available
int getNonReplaceableFrame();

//Returns the LFU frame to use for replacement
int getLfuFrameNum();

//Returns the value of the dirty bit of the given frame number
short unsigned getDirtyBitFrameTable(int index);

//Sets the value of the dirty bit of the given frame number to the value given as input by user
short unsigned setDirtyBitFrameTable(int index, int value);

//Returns the value of the empty bit of the given frame number
short unsigned getEmptyBitFrameTable(int index);

//Sets the value of the empty bit of the given frame number to the value given as input by user
short unsigned setEmptyBitFrameTable(int index, int value);

//Finds an empty frame and allocates it to a process.
//Takes segment number, page table level in which page fault occured and page index in the pafge table as input 
int allocateFrame(int pid,int segno,pageTable *pT,int pageNum,int level);

	
//Writes a dirty frame back to disk and sets its empty bit to zero.
int invalidateFrame(int frameNo);

//Initializes the ADT of the frame table
int initFrameTable();

//Runs the LFU frame ageing alogrithm 
int frameAgeing();

//Updates the frame access count which is used in LFU page replacement in the frame table
int updateLfuCount(int frameNo);

//Locks the Frame
int setLock(int frameNo);

//Function to read from memory and update the lfu count
int readFromMemory(int frameNo);

//Function to write to memory, update the lfu count and set dirty bit of the frame
int writeToMemory(int frameNo);
