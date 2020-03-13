/*
 * Author: Akhil
 * Date: 13/3/20
 *
 * Contains the ADT of the TLB.
 */

typedef struct 
{
	unsigned int validInvalidBit: 1;
	unsigned int modifiedBit: 1;
	unsigned int pageNum: 22;
	unsigned int frameNum: 16;
	unsigned int lruCount: 4;
}
TLBL1Entry;

typedef struct 
{
	unsigned int validInvalidBit: 1;
	unsigned int modifiedBit: 1;
	unsigned int pageNum: 22;
	unsigned int frameNum: 16;
	unsigned int lruCount: 5;
}
TLBL2Entry;

typedef struct
{
	TLBL1Entry entries[12];
}
TLBL1;


typedef struct 
{
	TLBL2Entry entries[24];
}
TLBL2;


// Functions
int TLBL1Flush();
unsigned int TLBL1Search(unsigned int pageNum, int *error); //Second parameter for the error.
int TLBL1Update(unsigned int pageNum, unsigned int frameNum);
int TLBL1UpdateLru(int index);
int TLBL1Getlru(); // Returns the index of the entry in the array.

int TLBL2Flush();
unsigned int TLBL2Search(unsigned int pageNum);
int TLBL2Update (unsigned int pageNum, unsigned int frameNum);
int TLBL2UpdateLru(int index);
int TLBL2Getlru(); // Returns the index of the entry in the array.