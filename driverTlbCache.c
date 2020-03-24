#include <stdio.h>

#include "configuration.h"
#include "cache.h"
#include "tlb.h"
#include "pcb.h"

#define NUM_LINES_BEFORE_CONTEXT_SWITCH 200
#define NUM_PROCESSES 5

TLBL1 l1TLB;
TLBL2 l2TLB;

CacheL1 l1Cache[2]; 
CacheL2 l2Cache;

int main(int argc, char const *argv[])
{
	// Initialising both the levels of the TLB and the cache.
	TLBL1Flush();
	TLBL2Flush();

	initL1Cache();
	initL2Cache();

	TLBL1Print();
	TLBL2Print();

	FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");

	int numPocessesAlive = NUM_PROCESSES;
	int inputAddr;
	int index, tag;
	int frameNum, error, segNum;
	bool write;
	int time, previousActionTime;
	long long int totalTime = 0;

	int requestedPageNum, requestedPageOffset;
	int l1CacheTag, l1CacheIndex, l2CacheTag, l2CacheIndex;
	int l1CacheOffset, l2CacheOffset;

	for (int i = 0; i < NUM_PROCESSES; ++i)
	{
		if (numPocessesAlive == 0)
		{
			break;
		}

		i = i % NUM_PROCESSES;

		if (getState(pcb[i]) != READY)
		{
			continue;
		}

		setState(pcb[i], RUNNING);
		fprintf(outputFile, "\n\n\nDriver: Process-%d running in the processor.\n", i);

		for (int j = 0; j < NUM_LINES_BEFORE_CONTEXT_SWITCH; ++j)
		{
			// Resetting at the start of every memory access.
			time = 0;

			readLinearAddr(pcb[i].LinearAddrInputFile, &inputAddr);

			readSegAddr(pcb[i].SegNumAddrFile, &segNum, &write);

			// If we have finished reading the file of that process.
			if (inputAddr == -1)
			{
				fprintf(outputFile, "\n\nDriver: Process-%d has finished execution.\n", i);
				setState(pcb[i], TERMINATED);
				numPocessesAlive--;
				break;
			}

			// Split the inputAddr to find the requested pageNum.
			requestedPageOffset = inputAddr & (pow(2, 10) - 1); // Since page size is 2**10B.
			requestedPageNum = inputAddr >> 10; // Since we need to discard the least significant 10b.

			

			// Search this pageNum in both levels of TLB.
			fprintf(outputFile, "Driver: Memory access for %x requested by process-%d\n", i);
			frameNum = TLBL1Search(requestedPageNum);

			time += L1_TLB_SEARCH_TIME;
			fprintf(outputFile, "Driver: Searched through L1 TLB. Time cost: %d\n", L1_TLB_SEARCH_TIME);
			if (frameNum < 0)
			{
				time += L2_TLB_SEARCH_TIME;
				fprintf(outputFile, "Driver: Did not find required data in L1 TLB. Searched through L2 TLB. Time cost: %d\n", L2_TLB_SEARCH_TIME);
				frameNum = TLBL2Search(requestedPageNum);

				if (frameNum < 0) // If the data is not present in TLBL2 also.
				{
					// Get the data from MM.

					// --------------------------------

					time += MM_SEARCH_TIME;
					fprintf(outputFile, "Driver: Did not find required data in L2 TLB. Searched through MM. Time cost: %d\n", MM_SEARCH_TIME);

					TLBL2Update(requestedPageNum, frameNum);
					time += L2_TLB_UPDATE_TIME;
					fprintf(outputFile, "Driver: Updated L2 TLB. Update time: %d\n", L2_TLB_UPDATE_TIME);

					frameNum = TLBL2Search(requestedPageNum);
					time += L2_TLB_SEARCH_TIME;
					fprintf(outputFile, "Driver: Re-searched through L2 TLB. Time cost: %d\n", L2_TLB_SEARCH_TIME);
				}

				TLBL1Update(requestedPageNum, frameNum);
				time += L1_TLB_UPDATE_TIME;
				fprintf(outputFile, "Driver: Updated L1 TLB. Update time: %d\n", L1_TLB_UPDATE_TIME);

				frameNum = TLBL1Search(requestedPageNum);
				time += L1_TLB_SEARCH_TIME;
				fprintf(outputFile, "Driver: Re-searched through L1 TLB. Time cost: %d\n", L1_TLB_SEARCH_TIME);

			}

			// Based on the MSB of the semNum, decide which cache to hit (data or instr).
			bool dataCache = (segNum[3] == 0) ? true : false;

			// Split the frameNum into index, tag and offset according to both levels of cache's size.
			l1CacheOffset = frameNum & (pow(2, 5) - 1);
			l1CacheIndex = (frameNum & (pow(2, 10) - 1)) >> 5;
			l1CacheTag = frameNum >> 10;

			l2CacheOffset = frameNum & (pow(2, 6) - 1);
			l1CacheIndex = (frameNum & (pow(2, 12) - 1)) >> 6;
			l1CacheTag = frameNum >> 12;

			
			int retValue;
			if (write)
			{
				// Since L1 cache is write through, the driver function only writes to the L1 cache.
				// And the writeL1Cache() calls writeL2Cache().
				retValue = writeL1Cache(l1CacheIndex, l1CacheTag, j, l2CacheIndex, l2CacheTag, dataCache);
				
				if (retValue < 0)
				{
					//-------------------------------
					// Error Codes.
					if (retValue == ERROR_WRITE_FAILED_NO_TAG_MATCH)
						// Do something.

					else if (retValue == ERROR_WRITE_FAILED_NO_PERMISSION)
						// Do someting.

					else if (retValue == ERROR_CANNOT_WRITE_IN_INSTR_CACHE)
						// Do something.

					//-------------------------------
				}
				else
				{
					time += L1_CACHE_WRITE_TIME;
					fprintf(outputFile, "Driver: Write to L1 Cache successfully completed.\n");
					continue;
				}
			}
			else // If it is a read.
			{

				// Since L1 cache is look aside, we essentially search L1 and L2 cache simultaneously.
				// But this cannot be shown in the simulation, hence we consider the time taken to do the search to be MIN() of the two of them.
				// For look-through we would have added the time they take to search.
				retValue1 = searchL1Cache(l1CacheIndex, l1CacheTag, dataCache);

				retValue2 = searchL2Cache(l2CacheIndex, l2CacheTag);

				if (retValue1 >= 0 && retValue2 >= 0)
				{
					// Return the data to the processor.
					//return retValue;

					previousActionTime = min(L1_CACHE_SEARCH_TIME, L2_CACHE_SEARCH_TIME);
					time += previousActionTime;

					fprintf(outputFile, "Driver: Found the required data in L1 cache.\n");
					fprintf(outputFile, "Search time taken: %d.\n", previousActionTime);
					continue;
				}
				else if (retValue1 < 0 && retValue2 < 0)
				{
					// Not there in both the levels of cache.

					previousActionTime = max(L1_CACHE_SEARCH_TIME, L2_CACHE_SEARCH_TIME);
					time += previousActionTime;

					fprintf(outputFile, "Driver: Did NOT find the required data in both L1 and L2 cache\n");
					fprintf(outputFile, "Search time taken: %d.\n", previousActionTime);
					fprintf(outputFile, "Driver: Will update L1 and L2 caches.\n");



					// search in the MM and update caches.

					//----------------------------
					// updateL2Cache();
					time += L2_CACHE_UPDATE_TIME;
					fprintf(outputFile, "Driver: L2 Cache update time: %d\n", L2_CACHE_UPDATE_TIME);

					// updateL1Cache();
					time += L1_CACHE_UPDATE_TIME;
					fprintf(outputFile, "Driver: L1 Cache update time: %d\n", L1_CACHE_UPDATE_TIME);
				}
				else if (retValue1 < 0 && retValue2 >= 0)
				{
					// Present in L2 cache, but not in L1.

					previousActionTime = max(L1_CACHE_SEARCH_TIME, L2_CACHE_SEARCH_TIME);
					time += previousActionTime;
					fprintf(outputFile, "Driver: Did NOT find the required data in L1 cache, but found it in L2 cache.\n");
					fprintf(outputFile, "Search time taken: %d.\n", previousActionTime);
					fprintf(outputFile, "Driver: Will update L1 Cache.\n");

					// Update L1 cache.
					time += L1_CACHE_UPDATE_TIME;

					// Update the LRU in L1 cache if required.
					// ----------------------------

					fprintf(outputFile, "Driver: L1 Cache update time: %d\n", L1_CACHE_UPDATE_TIME);

					continue;

				}
			} // End of that memory access.

			// Updating the time taken by the previous memory access.
			totalTime += time;
			pcb[i].runTime += time;
		
		} // End of that process' quota of memory accesses.

		fprintf(outputFile, "Driver: Process-%d being preempted.\n", i);
		setState(pcb[i], READY);

	} // End of all processes and memory accesses.

	fprintf(outputFile, "\n\n\n\nDriver: All the processes have finished executing. Total Time taken: %lld Units.\n", totalTime);
	for (int i = 0; i < NUM_PROCESSES; ++i)
	{
		fprintf(outputFile, "Process-%d ran for: %lld\n", pcb[i].runTime);
	}

	fclose(outputFile);
	return 0;
}