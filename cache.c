/*
 * Author: Akhil
 * Date: 19/3/20
 *
 * Contains the function definiions of the cache ADT.
 */

#include <stdio.h>
#include "cache.h"

// L1 Cache is split into two-- data and instruction.
CacheL1 l1Cache[2]; 
CacheL2 l2Cache;



int main(int argc, char const *argv[])
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "w");
	fclose(outputFile);

	initL2Cache();

	int setIndex = 3;
	printL2Cache(setIndex);
	
	return 0;
}



// -----------------------------------------------------------------
// L1-Cache


/*
 * Initialises all the sets and ways to invalid entries.
 */
void
initL1Cache()
{
	for (int i = 0; i < NUM_SETS_IN_L1_CACHE; ++i)
	{
		for (int j = 0; j < NUM_WAYS_IN_L1_CACHE; ++j)
		{
			l1Cache[0].sets[i].ways[j].validInvalidBit = 0;
			l1Cache[1].sets[i].ways[j].validInvalidBit = 0;
		}
	}
}


/*
 * Prints the particular set of L1 cache in its current state.
 */
void
printL1Cache(int setIndex, bool dataCache)
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fprintf(outputFile, "-------------------------\n");
	fprintf(outputFile, "L1-Cache Info\n");

	int index;
	if (dataCache)
		index = 0;
	else
		index = 1;

	fprintf(outputFile, "Set number=%d\n", setIndex);
	for (int j = 0; j < NUM_WAYS_IN_L1_CACHE; ++j)
	{
		fprintf(outputFile, "\tWay number=%d, validInvalidBit=%d, readWriteBit=%d\n", j, l1Cache[index].sets[setIndex].ways[j].validInvalidBit, l1Cache[index].sets[setIndex].ways[j].readWriteBit);
	}


	fclose(outputFile);

	return;
}





// -----------------------------------------------------------------
// L2-Cache


/*
 * Initialises all the sets and ways to invalid entries.
 */
void
initL2Cache()
{
	for (int i = 0; i < NUM_SETS_IN_L2_CACHE; ++i)
	{
		for (int j = 0; j < NUM_WAYS_IN_L2_CACHE; ++j)
		{
			l2Cache.sets[i].ways[j].validInvalidBit = 0;
		}
	}
}



/*
 * Prints the particular set of L2 cache in its current state.
 */
void
printL2Cache(int setIndex)
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fprintf(outputFile, "-------------------------\n");
	fprintf(outputFile, "L2-Cache Info\n");

	fprintf(outputFile, "Set number=%d\n", setIndex);
	for (int j = 0; j < NUM_WAYS_IN_L2_CACHE; ++j)
	{
		fprintf(outputFile, "\tWay number=%d, validInvalidBit=%d, dirtyBit=%d, readWriteBit=%d, lruCount=%d\n", j, l2Cache.sets[setIndex].ways[j].validInvalidBit, l2Cache.sets[setIndex].ways[j].dirtyBit, l2Cache.sets[setIndex].ways[j].readWriteBit, l2Cache.sets[setIndex].ways[j].lruCount);
	}


	fclose(outputFile);

	return;
}




/*
 * Returns the data if its present.
 */
int
searchL2Cache(int index, int tag)
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fprintf(outputFile, "\nL2-Cache: Seaching for index=%d and tag=%d\n", index, tag);

	for (int i = 0; i < NUM_WAYS_IN_L2_CACHE; ++i)
	{
		if (l2Cache.sets[index].ways[i].validInvalidBit && l2Cache.sets[index].ways[i].tag == tag)
		{
			fprintf(outputFile, "L2-Cache: Found index=%d, tag=%d in way=%d\n", index, tag, i);
			fclose(outputFile);
			// Data is 0
			return 0;
		}
	}

	fprintf(outputFile, "L2-Cache: Did not find index=%d and tag=%d\n", index, tag);
	fclose(outputFile);

	// If data is not found.
	return -1;
}


/*
 * Returns the way number of the LRU way in the given set number.
 */
int
getLruL2Cache(int setIndex)
{
	int minValue = MAX_VALUE_IN_8_BITS;
	int minIndex = -1;

	for (int i = 0; i < NUM_WAYS_IN_L2_CACHE; ++i)
	{
		if (l2Cache.sets[setIndex].ways[i].lruCount < minValue)
		{
			minIndex = i;
			minValue = l2Cache.sets[setIndex].ways[i].lruCount;
		}
	}

	return minIndex;
}


/*
 * Sets the MSB of that way and that set in L2 cache.
 */
void
updateLruL2Cache(int setIndex, int wayIndex)
{
	int currentValue = l2Cache.sets[setIndex].ways[wayIndex].lruCount;

	currentValue = currentValue & VALUE_OF_8_BITS_WITH_FIRST_BIT_SET;

	l2Cache.sets[setIndex].ways[wayIndex].lruCount = currentValue;
}



/*
 * Writes some data into the cache. 
 * Returns 1 on success.
 * Returns negative numbers on failure.
 */
int
writeL2Cache(int index, int tag, int data)
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fprintf(outputFile, "L2-Cache: Writing data onto index=%d, tag=%d\n", index, tag);

	for (int i = 0; i < NUM_WAYS_IN_L2_CACHE; ++i)
	{
		if (l2Cache.sets[index].ways[i].validInvalidBit && l2Cache.sets[index].ways[i].tag == tag)
		{
			// Writing data.
			// l2Cache.sets[index].ways[i].data = data;
			fprintf(outputFile, "L2-Cache: Writing data onto index=%d, tag=%d in way=%d\n", index, tag, i);
			fclose(outputFile);
			return 1;
		}
	}

	fprintf(outputFile, "L2-Cache: Write FAILED as none of the valid block's tag matched.\n");

	fclose(outputFile);
	return ERROR_WRITE_FAILED;
}