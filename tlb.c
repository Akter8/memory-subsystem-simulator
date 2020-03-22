/*
 * Author: Akhil
 * Date: 18/3/20
 *
 * Contains the function definiions of the TLB ADT.
 */

#include <stdio.h>
#include "tlb.h"

extern inline void fileNotNull(FILE *file, char *fileName);

TLBL1 l1TLB;
TLBL2 l2TLB;


// To test the functions.
/*
int
main(int argc, char const *argv[])
{

	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "w");
	fclose(outputFile);
	

	outputFile = fopen(OUTPUT_FILE_NAME, "a");

	TLBL1Flush();

	TLBL1Print();

	TLBL2Flush();

	TLBL2Print();

	int error;
	int pageNum = 0;
	int frameNum;
	frameNum = TLBL1Search(pageNum, &error);
	frameNum = TLBL2Search(pageNum, &error);


	int count, index;
	index = 0;
	count = TLBL1UpdateLru(index);
	printf("count=%d\n", count);
	index = TLBL1GetLruIndex();
	printf("index=%d\n", index);

	index = 4;
	count = TLBL1UpdateLru(index);

	index = TLBL1Update(1, 1);

	TLBL1Print();

	fclose(outputFile);
	return 0;
}
*/

// -----------------------------------------------------------------
// L1-TLB

/*
 * This function invalidates all the entries in the L1 TLB.
 */
void
TLBL1Flush()
{
	for (int i = 0; i < NUM_ENTRIES_IN_L1_TLB; ++i)
	{
		// Setting the validInvalid to 0 makes the entry invalid.
		l1TLB.entries[i].validInvalidBit = 0;
	}

	return;
}


/*
 * This function prints the current data in the L1 TLB.
 */
void
TLBL1Print()
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fileNotNull(outputFile, OUTPUT_FILE_NAME);

	fprintf(outputFile, "-------------------------\n");
	fprintf(outputFile, "L1-TLB Info\n");

	for (int i = 0; i < NUM_ENTRIES_IN_L1_TLB; ++i) // Iterates through every entry in the TLB.
	{
		fprintf(outputFile, "Index=%d, validInvalidBit=%d, pageNum=%d, frameNum=%d, lruCount=%d\n", i, l1TLB.entries[i].validInvalidBit, l1TLB.entries[i].pageNum, l1TLB.entries[i].frameNum, l1TLB.entries[i].lruCount);
	}

	fclose(outputFile);

	return;
}



/*
 * For a given pageNum, it returns the frame number if a valid record for that pageNum exists.
 */
unsigned int 
TLBL1Search(unsigned int pageNum, unsigned int *error)
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fileNotNull(outputFile, OUTPUT_FILE_NAME);

	fprintf(outputFile, "\nL1-TLB: Searching for pageNum=%d\n", pageNum);

	for (int i = 0; i < NUM_ENTRIES_IN_L1_TLB; ++i)
	{
		// The entry has to be valid and it also has to match the pageNum we are looking for.
		if (l1TLB.entries[i].validInvalidBit && pageNum == l1TLB.entries[i].pageNum)
		{
			fprintf(outputFile, "pageNum=%d found in l1TLB at index=%d, frameNum=%d\n", pageNum, i, l1TLB.entries[i].frameNum);
			fflush(outputFile);
			fclose(outputFile);

			// Update the LRU counts.
			TLBL1UpdateLru(i);

			*error = 0;
			return l1TLB.entries[i].frameNum;
		}
	}

	fprintf(outputFile, "pageNum=%d not found in l1TLB\n", pageNum);
	fclose(outputFile);

	*error = ERROR_PAGE_NUM_NOT_FOUND;
	return 0;
}


/*
 * This function returns the LRU index in L1-TLB.
 * This function does not check if the LRU is valid or not.
 * It also does not check for placement before replacement.
 * Following the counter implementation of LRU.
 */
int
TLBL1GetLruIndex()
{
	for (int i = 0; i < NUM_ENTRIES_IN_L1_TLB; ++i)
	{
		// Counter implementation of LRU makes the LRU have a count of 0.
		if (l1TLB.entries[i].lruCount == 0)
		{
			return i;
		}
	}
}


/*
 * This function updates the LRU counts in L1-TLB.
 * This function does not care about the validInvalidBit.
 * Returns the number of entries which had a greater LRU count.
 * Counter implementation of LRU.
 */
int
TLBL1UpdateLru(int index)
{
	int currentCount = l1TLB.entries[index].lruCount;
	int count = 0;

	for (int i = 0; i < NUM_ENTRIES_IN_L1_TLB; ++i)
	{
		// Counter implementation of LRU requires all LRU counts greater than the one being accessed now to be decremented.
	 	if (l1TLB.entries[i].lruCount > currentCount)
	 	{
	 		l1TLB.entries[i].lruCount--;
	 		count++;
	 	}
	}

	l1TLB.entries[index].lruCount = NUM_ENTRIES_IN_L1_TLB - 1;
	return count;
}


/*
 * Returns the index of the first entry that is invalid.
 * If not found, returns -1.
 */
int
TLBL1GetFirstInvalidEntry()
{
	for (int i = 0; i < NUM_ENTRIES_IN_L1_TLB; ++i)
	{
		if (l1TLB.entries[i].validInvalidBit == 0)
		{
			return i;
		}
	}

	return -1;
}


/*
 * Finds an entry in the L1-TLB and writes onto it.
 * Returns the index of the new valid entry.
 */
int
TLBL1Update(unsigned int pageNum, unsigned int frameNum)
{
	/* Algorithm
	Placement first and then replacement.
	To find if placement if possible, call GetFirstInvalidEntry()
	If index != -1:
		// Placement
		Use the index that was returned.
	Else:
		// Replacement
		Find the LRU index and replace it.
	Else
	End */

	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fileNotNull(outputFile, OUTPUT_FILE_NAME);


	// Finding the first invalid entry in the TLB for placement.
	int index = TLBL1GetFirstInvalidEntry();

	// Finding the LRU index if no invalid entries.
	if (index == -1) // Replacement.
	{
		index = TLBL1GetLruIndex();
		fprintf(outputFile, "L1-TLB: Replacement in index=%d\n", index);
	}
	else // Placement.
	{
		fprintf(outputFile, "L1-TLB: Placement in index=%d\n", index);
	}

	fclose(outputFile);

	// Initialising the new entry.
	l1TLB.entries[index].frameNum = frameNum;
	l1TLB.entries[index].pageNum = pageNum;
	l1TLB.entries[index].validInvalidBit = 1;

	// We do not change the LRU count as of now, but when the same instruction runs again, it will be changed then.

	return index;
}


// -----------------------------------------------------------------
// L2-TLB

/*
 * This function invalidates all the entries in the L2 TLB.
 */
void
TLBL2Flush()
{
	for (int i = 0; i < NUM_ENTRIES_IN_L2_TLB; ++i)
	{
		l2TLB.entries[i].validInvalidBit = 0;
	}

	return;
}


/*
 * This function prints the current data in the L2 TLB.
 */
void
TLBL2Print()
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fileNotNull(outputFile, OUTPUT_FILE_NAME);

	fprintf(outputFile, "-------------------------\n");
	fprintf(outputFile, "L2-TLB Info\n");

	for (int i = 0; i < NUM_ENTRIES_IN_L2_TLB; ++i)
	{
		fprintf(outputFile, "Index=%d, validInvalidBit=%d, pageNum=%d, frameNum=%d, lruCount=%d\n", i, l2TLB.entries[i].validInvalidBit, l2TLB.entries[i].pageNum, l2TLB.entries[i].frameNum, l2TLB.entries[i].lruCount);
	}
	fflush(outputFile);
	fclose(outputFile);

	return;
}


/*
 * For a given pageNum, it returns the frame number if a valid record for that pageNum exists.
 */
unsigned int 
TLBL2Search(unsigned int pageNum, unsigned int *error)
{
	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fileNotNull(outputFile, OUTPUT_FILE_NAME);
	fprintf(outputFile, "\nL2-TLB: Searching for pageNum=%d\n", pageNum);

	for (int i = 0; i < NUM_ENTRIES_IN_L2_TLB; ++i)
	{
		if (l2TLB.entries[i].validInvalidBit && pageNum == l2TLB.entries[i].pageNum)
		{
			fprintf(outputFile, "L2-TLB: pageNum=%d found in l2TLB at index=%d, frameNum=%d\n", pageNum, i, l2TLB.entries[i].frameNum);
			fflush(outputFile);
			fclose(outputFile);

			// Update the LRU counts.
			TLBL2UpdateLru(i);

			*error = 0;
			return l2TLB.entries[i].frameNum;
		}
	}

	fprintf(outputFile, "L2-TLB: pageNum=%d not found in l2TLB\n", pageNum);

	fflush(outputFile);
	fclose(outputFile);

	*error = ERROR_PAGE_NUM_NOT_FOUND;
	return 0;
}


/*
 * This function returns the LRU index in L2-TLB.
 * This function does not check if the LRU is valid or not.
 * It also does not check for placement before replacement.
 * Counter implementation of LRU.
 */
int
TLBL2GetLruIndex()
{
	for (int i = 0; i < NUM_ENTRIES_IN_L2_TLB; ++i)
	{
		if (l2TLB.entries[i].lruCount == 0)
		{
			return i;
		}
	}
}



/*
 * This function updates the LRU counts in L2-TLB.
 * This function does not care about the validInvalidBit.
 * Returns the number of entries which had a greater LRU count.
 * Counter implementation of LRU.
 */
int
TLBL2UpdateLru(int index)
{
	int currentCount = l2TLB.entries[index].lruCount;
	int count = 0;

	for (int i = 0; i < NUM_ENTRIES_IN_L2_TLB; ++i)
	{
		// Decrement LRU values of those ones which have a greater count than the current one. 
	 	if (l2TLB.entries[i].lruCount > currentCount)
	 	{
	 		l2TLB.entries[i].lruCount--;
	 		count++;
	 	}
	}

	l2TLB.entries[index].lruCount = NUM_ENTRIES_IN_L2_TLB - 1;
	return count;
}



/*
 * Returns the index of the first entry that is invalid.
 * If not found, returns -1.
 */
int
TLBL2GetFirstInvalidEntry()
{
	for (int i = 0; i < NUM_ENTRIES_IN_L2_TLB; ++i)
	{
		if (l2TLB.entries[i].validInvalidBit == 0)
		{
			return i;
		}
	}

	return -1;
}


/*
 * Finds an entry in the L2-TLB and writes onto it.
 * Returns the index of the new valid entry.
 */
int
TLBL2Update(unsigned int pageNum, unsigned int frameNum)
{
	/* Algorithm
	Placement first and then replacement.
	To find if placement if possible, call GetFirstInvalidEntry()
	If index != -1:
		// Placement
		Use the index that was returned.
	Else:
		// Replacement
		Find the LRU index and replace it.
	Else
	End */

	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");
	fileNotNull(outputFile, OUTPUT_FILE_NAME);

	// Finding the first invalid entry to see if placement is possible.
	int index = TLBL2GetFirstInvalidEntry();

	// Finding the LRU index if no invalid entries.
	if (index == -1) // Replacement.
	{
		index = TLBL2GetLruIndex();
		fprintf(outputFile, "L2-TLB: Replacement in index=%d\n", index);
	}
	else
	{
		fprintf(outputFile, "L2-TLB: Placement in index=%d\n", index);
	}

	fclose(outputFile);

	l2TLB.entries[index].frameNum = frameNum;
	l2TLB.entries[index].pageNum = pageNum;
	l2TLB.entries[index].validInvalidBit = 1;

	// We do not change the LRU count as of now, but when the same instruction runs again, it will be changed then.

	return index;
}