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

	int setIndex, wayIndex;
	setIndex = 3; wayIndex = 4;

	printL2Cache(setIndex);

	updateL2Cache(setIndex, 100, true, 100);
	updateL2Cache(setIndex, 101, true, 101);
	updateL2Cache(setIndex, 102, true, 102);
	updateL2Cache(setIndex, 103, true, 103);
	updateL2Cache(setIndex, 104, true, 104);
	updateL2Cache(setIndex, 105, true, 105);
	updateL2Cache(setIndex, 106, true, 106);
	updateL2Cache(setIndex, 107, true, 107);

	searchL2Cache(setIndex, 100);
	searchL2Cache(setIndex, 101);

	updateL2Cache(setIndex, 108, false, 108);

	searchL2Cache(setIndex, 106);

	printL2Cache(setIndex);

	writeL2Cache(setIndex, 108, 11);
	writeL2Cache(setIndex, 103, 10);

	printL2Cache(setIndex);

	searchL2Cache(setIndex, 110);

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

	int index = (dataCache == true) ? 0 : 1;

	fprintf(outputFile, "Set number=%d\n", setIndex);
	for (int j = 0; j < NUM_WAYS_IN_L1_CACHE; ++j)
	{
		fprintf(outputFile, "\tWay number=%d, tag=%d, validInvalidBit=%d, readWriteBit=%d\n", j, l1Cache[index].sets[setIndex].ways[j].tag,l1Cache[index].sets[setIndex].ways[j].validInvalidBit, l1Cache[index].sets[setIndex].ways[j].readWriteBit);
	}

	fclose(outputFile);

	return;
}


/*
 * Returns the way number of the LRU way in the given set number.
 * In the square matrix implementation the LRU is the one which has its entire row as 0s.
 */
int
getLruIndexL1Cache(int setIndex, bool dataCache)
{
	int index = (dataCache == true) ? 0 : 1;

	for (int i = 0; i < NUM_WAYS_IN_L1_CACHE; ++i) // Iterates through the rows of the square matrix.
	{
		bool flag = true;
		for (int j = 0; j < NUM_WAYS_IN_L1_CACHE; ++j) // Iterates through the columns of the square matrix.
		{
			// If a way is LRU, then all the entries in its row will be 0.
			if (l1Cache[index].sets[setIndex].lruSquareMatrix[i][j] == 1)
			{
				flag = false;
				break;
			}
		}

		if (flag)
		{
			return i;
		}
	}

	return -1;
}



/*
 *
 *
 */
void 
updateLruL1Cache(int setIndex, int wayIndex, bool dataCache)
{
	int index = (dataCache == true) ? 0 : 1;

	for (int i = 0; i < NUM_WAYS_IN_L1_CACHE; ++i)
	{
		l1Cache[index].sets[setIndex].lruSquareMatrix[wayIndex][i] = 1;
	}

	for (int i = 0; i < NUM_WAYS_IN_L1_CACHE; ++i)
	{
		l1Cache[index].sets[setIndex].lruSquareMatrix[i][wayIndex] = 0;
	}


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
		fprintf(outputFile, "\tWay number=%d, tag=%d, validInvalidBit=%d, dirtyBit=%d, readWriteBit=%d, lruCount=%d\n", j, l2Cache.sets[setIndex].ways[j].tag, l2Cache.sets[setIndex].ways[j].validInvalidBit, l2Cache.sets[setIndex].ways[j].dirtyBit, l2Cache.sets[setIndex].ways[j].readWriteBit, l2Cache.sets[setIndex].ways[j].lruCount);
	}


	fclose(outputFile);

	return;
}


/*
 * Returns the way number of the LRU way in the given set number.
 * In the counter implementation the LRU is the one which has its LRU count as 0.
 */
int
getLruIndexL2Cache(int setIndex)
{
	for (int i = 0; i < NUM_WAYS_IN_L2_CACHE; ++i)
	{
		if (l2Cache.sets[setIndex].ways[i].lruCount == 0)
		{
			return i;
		}
	}

	return -1;
}


/*
 * Sets the MSB of that way and that set in L2 cache.
 * Counter implementation is such that the ways which have a greater count than the current one will be decremented.
 */
void
updateLruL2Cache(int setIndex, int wayIndex)
{
	int currentLruCount = l2Cache.sets[setIndex].ways[wayIndex].lruCount;

	for (int i = 0; i < NUM_WAYS_IN_L2_CACHE; ++i)
	{
		if (l2Cache.sets[setIndex].ways[i].lruCount > currentLruCount)
		{
			l2Cache.sets[setIndex].ways[i].lruCount--;
		}
	}

	l2Cache.sets[setIndex].ways[wayIndex].lruCount = NUM_WAYS_IN_L2_CACHE - 1;
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

			// Update the LRU count.
			updateLruL2Cache(index, i);

			// Data is 0.
			return 0;
		}
	}

	fprintf(outputFile, "L2-Cache: Did not find index=%d and tag=%d\n", index, tag);
	fprintf(outputFile, "L2-Cache: Will update the cache and re-search the required data.\n");
	fclose(outputFile);

	// Updating the cache.
	updateL2Cache(index, tag, false, 0);

	// Calling search() function to find it now.
	searchL2Cache(index, tag);

	// If data is not found.
	return -1;
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
	fprintf(outputFile, "\nL2-Cache: Writing data onto index=%d, tag=%d\n", index, tag);

	for (int i = 0; i < NUM_WAYS_IN_L2_CACHE; ++i)
	{
		if (l2Cache.sets[index].ways[i].validInvalidBit && l2Cache.sets[index].ways[i].tag == tag)
		{
			if (l2Cache.sets[index].ways[i].readWriteBit == 0)
			{
				fprintf(outputFile, "L2-Cache: Write FAILED as there is no permission to write in index=%d, tag=%d.\n", index, tag);
				fclose(outputFile);
				return ERROR_WRITE_FAILED_NO_PERMISSION;
			}

			// Writing data.
			// l2Cache.sets[index].ways[i].data = data;
			fprintf(outputFile, "L2-Cache: Writing data onto index=%d, tag=%d in way=%d\n", index, tag, i);
			fclose(outputFile);

			// Updating LRU.
			updateLruL2Cache(index, i);

			return 1;
		}
	}

	fprintf(outputFile, "L2-Cache: Write FAILED as none of the valid block's tag matched.\n");

	fclose(outputFile);
	return ERROR_WRITE_FAILED_NO_TAG_MATCH;

	// Not calling updateL2Cache() because the L1 cache will only have this data if its there in L2. There is no way one can write without reading. So this last error case should never happen. 
}



/*
 * Returns the index of the first invalid way, else returns -1
 */
int
getFirstInvalidWayL2Cache(int setIndex)
{
	for (int i = 0; i < NUM_WAYS_IN_L2_CACHE; ++i)
	{
		if (l2Cache.sets[setIndex].ways[i].validInvalidBit == 0)
		{
			return i;
		}
	}

	return -1;
}


/*
 * Finds an empty cache way in that set if possible and stores the new data.
 * If no empty way is found for that set, then we replace the LRU way.
 * Follows placement and only then replacement.
 * Returns the way number it is stored in.
 */
int
updateL2Cache(int index, int tag, bool write, int data)
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");

	int way = getFirstInvalidWayL2Cache(index);

	if (way == -1) // Did not find an invalid entry. Replacement required.
	{
		way = getLruIndexL2Cache(index);
		fprintf(outputFile, "L2-Cache: Replacement in set=%d, way=%d\n", index, way);
	}
	else
	{
		fprintf(outputFile, "L2-Cache: Placement in set=%d, way=%d\n", index, way);
	}

	fclose(outputFile);

	// Updating the data.
	// l2Cache.sets[index].ways[way].data = data;

	// Initialising the way for this new entry.
	l2Cache.sets[index].ways[way].tag = tag;
	l2Cache.sets[index].ways[way].validInvalidBit = 1;
	l2Cache.sets[index].ways[way].dirtyBit = 0;
	l2Cache.sets[index].ways[way].readWriteBit = (write == false) ? 0 : 1;


	return way;
}