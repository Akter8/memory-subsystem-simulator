#include <stdio.h>
#include "cache.h"
#include "configuration.h"
#include <stdlib.h>
//#include "dataTypes.h"
//#include "frameTable.h"
//#include "pageTable.h"
#include "pcb.h"
//#include "segmentTable.h"
//#include "tlb.h"
#include "utility.h"
#define NUM_LINES_BEFORE_CONTEXT_SWITCH 200

PCB pcbObj[30];
segmentTable* GDT;

//TLB objects of L1 and L2
TLBL1 tlbL1Obj;
TLBL2 tlbL2Obj;

//Cache objects L1 and L2
CacheL1 cacheL1obj[2];   //L1 instruction and data cache
CacheL2 cacheL2obj;

frameTable frameTableObj;

long long current_time = 0;

int main()
{
    int n;      //Number of processes
    scanf("%d", &n);


    char SegAddrInputFileName[n][100];
    char LinearAddrInputFileName[n][100];
    for(int i = 0; i < n; ++i)
    {
        scanf("%s", LinearAddrInputFileName[i]); 
    }
    for(int i = 0; i < n; ++i)
    {
        scanf("%s", SegAddrInputFileName[i]);
    }
   
    
    //Flushing the TLBs initially
    TLBL1Flush();
    TLBL2Flush();


    //Initializing Cache
    initL1Cache();
    initL2Cache();


    //Initialize Frame Table
    initFrameTable();


    //Global descriptor Table initialize
    GDT = initGDTable();

    //Initializing PCBs of all Processes
    //Initialize segment Table for each Process
    for(int i = 0; i < n; ++i)
    {
        //Initializes PCB
        initPCB(pcbObj[i], LinearAddrInputFileName[i], SegAddrInputFileName[i]);
    }

    //Opening outputFile
    FILE *outputFile = fopen(OUTPUT_FILE_NAME, "a");


    for(int i = 0; i < n; ++i)
    {
        if(numProcessAlive == 0)
        {
            break;
        }
        //Changes the status of the process
        if(getState(PCB[i]) == TERMINATED)
        {
            ++current_time;
            continue;
        }
        else if(getState(PCB[i]) == WAIT)
        {
            ++current_time;
            if(current_time-swapStartTime < WAITING_TIME)
            {
                continue;
            }
            else
            {
                setState(PCB[i], READY);
                ++current_time;
            }
        }

        setState(PCB[i], RUNNING);
        ++current_time;
        fprintf(outputFile, "\n\n\nDriver: Process-%d running in the processor.\n", i);



        for(int j = 0; j < NUM_LINES_BEFORE_CONTEXT_SWITCH; ++j)
        {
            //time initialized to zero after every context switch
            long time = 0;

            //Reads the Memory reference from input file
            unsignned int inputAddr = readAddr(PCB[i].LinearAddrInputFile[i], Addr);

            //Store Segment Number and whether read/write
            char write;
            int4 segNum = readSegNum(PCB[i].SegNumInputFile[i], &write);
             
            if(inputAddr == -1)     //To check if we have reached the end of the file
            {
                setState(PCB[i], TERMINATED);    
                --numProcessAlive;
                break;
            }


            //First we find the correct mapping of Logical Address to Physical Address

            // Split the inputAddr to find the requested pageNum.
			int requestedPageOffset = inputAddr & 1023; // (pow(2, 10) - 1), Since page size is 2**10B.
			int requestedPageNum = inputAddr >> 10; // Since we need to discard the least significant 10b.


            // Based on the MSB of the segNum, decide which cache to hit (data or instr).
            bool dataCache = (segNum == 0) ? true : false;


            // Search this pageNum in both levels of TLB.
			fprintf(outputFile, "Driver: Memory access for %x requested by process-%d\n", Addr, i);
			int frameNum = TLBL1Search(requestedPageNum);

            time += L1_TLB_SEARCH_TIME;
            fprintf(outputFile, "Driver: Searched through L1 TLB. Time cost: %d\n", L1_TLB_SEARCH_TIME);

            if (frameNum < 0)
			{
				time += L2_TLB_SEARCH_TIME;
				fprintf(outputFile, "Driver: Did not find required data in L1 TLB. Searched through L2 TLB. Time cost: %d\n", L2_TLB_SEARCH_TIME);
				frameNum = TLBL2Search(requestedPageNum);

					// Get the data from MM.

					// --------------------------------

				if (frameNum < 0) // If the data is not present in TLBL2 also.
				{
                    int level = 3;
                    if(dataCache)        //if memory reference is data
                    {
                        //
                        frameNum = searchPageTable(PCB[i].segTableObj->entries[segNum], ptrToPageFaultTable, inputAddr, &requestedPageNum, &level);
                        printf("After returing to driver searchPageTable: level = %d, pageFaultPageNumber = %d\n", level, &requestedPageNo);
                        //allocateFrame(i, segNum, *ptrToPageFaultPageTable, &requestedpageNo, &level);

                        if(frameNum == -2)
                        {
                            //Invalid Address
                            continue;
                        }

                        if(frameNum < 0)
                        {
                            //allocateFrame()
                            //fseek() for previous instruction
                            
                            break;

                            //level = 2;
                            //frameNum = searchPageTable(PCB[i].segTableObj->entries[segNum], ptrToPageFaultTable, inputAddr, requestedPageNum, &level);
                            //allocateFrame(i, segNum, *ptrToPageFaultPageTable, &requestedpageNo, &level);
                            //if(frameNum < 0)
                            //{
                            //    level = 1;
                            //    frameNum = searchPageTable(PCB[i].segTableObj->entries[segNum], ptrToPageFaultTable, inputAddr, requestedPageNum, &level);
                            //    allocateFrame(i, segNum, *ptrToPageFaultPageTable, &requestedpageNo, &level);
                            //}
                        }

                    }
                    //else        //if memory reference is an instruction
                    //{
                    //    frameNum = searchPageTable(GDT[PCB[i].GDTindex]->pageTableObj, ptrToPageFaultTable, inputAddr, &requestedPageNum, &level);
                    //    printf("After returing to driver searchPageTable: level = %d, pageFaultPageNumber = %d\n", level, &requestedPageNo);
                    //    allocateFrame(i, segNum, *ptrToPageFaultPageTable, &requestedpageNo, &level);

                    //    if(frameNum == -2)
                    //    {
                    //        //Invalid Address
                    //        continue;
                    //    }
                    //    if(frameNum < 0)
                    //    {
                    //        level = 2;
                    //        frameNum = searchPageTable(GDT[PCB[i].GDTindex], ptrToPageFaultTable, inputAddr, requestedPageNum, &level);
                    //        allocateFrame(i, segNum, *ptrToPageFaultPageTable, &requestedpageNo, &level);
                    //        if(frameNum < 0)
                    //        {
                    //            level = 1;
                    //            frameNum = searchPageTable(GDT[PCB[i].GDTindex], ptrToPageFaultTable, inputAddr, requestedPageNum, &level);
                    //            allocateFrame(i, segNum, *ptrToPageFaultPageTable, &requestedpageNo, &level);
                    //        }
                    //    }
                    //}


                    fprintf(outputFile, "Driver: Searched through PageTable, found the frameNumber for the required memory reference\n");


					time += MM_SEARCH_TIME;
					fprintf(outputFile, "Driver: Did not find required Addr mapping in L2 TLB. Searched through MM PageTable. Time cost: %d\n", MM_SEARCH_TIME);

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

            //Physical Addr (Frame Number) obtained. Now search for data
            physicalAddr = (frameNum << 10) & (inputAddr & (pow(2, 10) - 1));


			// Split the frameNum into index, tag and offset according to both levels of cache's size.
			l1CacheOffset = physicalAddr & (pow(2, 5) - 1);
			l1CacheIndex = (physicalAddr & (pow(2, 10) - 1)) >> 5;
			l1CacheTag = physicalAddr >> 10;

			l2CacheOffset = physicalAddr & (pow(2, 6) - 1);
			l2CacheIndex = (physicalAddr & (pow(2, 12) - 1)) >> 6;
			l2CacheTag = physicalAddr >> 12;


            int retValue;
			if (write)
			{
				// Since L1 cache is write through, the driver function only writes to the L1 cache.
				// And the writeL1Cache() calls writeL2Cache().
				retValue = writeL1Cache(l1CacheIndex, l1CacheTag, j, l2CacheIndex, l2CacheTag, dataCache);
                time += L1_CACHE_WRITE_TIME;
				
				if (retValue < 0)
				{
					//-------------------------------
					// Error Codes.
					if (retValue == ERROR_WRITE_FAILED_NO_TAG_MATCH)
                    {
                        fprintf(outputFile, "Driver: Write to L1 Cache Failed. Tag did not match. Time cost: %d\n", L1_CACHE_WRITE_TIME);
                        retValue = searchL2Cache(l2CacheIndex, l2CacheTag);
                        time += L2_CACHE_SEARCH_TIME;

                        if(retValue < 0)
                        {
                            fprintf(outputFile, "Driver: Search in L2 failed! Time cost: %d\n", L2_CACHE_SEARCH_TIME);
                            // Write to Main Memory
                            status = writeInMainMemory(physicalAddr);
                            time += MAIN_MEMORY_WRITE_TIME;
                            fprintf(outputFile, "Driver: Write in Main Memory successfull! %d\n",MAIN_MEMORY_WRITE_TIME);


                            if(status == ERROR_WRITE_FAILED_NO_PERMISSION) 
                            {
                                fprintf(outputFile, "Driver: Error! write permission not for this memory address\n");
                                continue;
                            }
                                
                            updateL2Cache(l2CacheIndex, l2CacheTag, write, 0, dataCache);
                            time += L2_CACHE_UPDATE_TIME;
                            fprintf(outputFile, "Driver: Updated L2 Cache. Time cost: %d\n", L2_CACHE_WRITE_TIME);

                            updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
                            time += L1_CACHE_UPDATE_TIME;
                            fprintf(outputFile, "Driver: Updated L1 Cache. Time cost: %d\n", L1_CACHE_WRITE_TIME);

                            writeL1Cache(l1CacheIndex, l1CacheTag, 0, dataCache);
                            time += L1_CACHE_WRITE_TIME;
                            fprintf(outputFile, "Driver: Write to L1 Cache successfully completed.\n");
                        }
                        else
                        {
                            fprintf(outputFile, "Driver: Search in L2 successfull! Search Time: %d\n", L2_CACHE_SEARCH_TIME);
                            updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
                            time += L1_CACHE_UPDATE_TIME;
                            fprintf(outputFile, "Driver: Updated L1 Cache. Time cost: %d\n", L1_CACHE_WRITE_TIME);

                            writeL1Cache(l1CacheIndex, l1CacheTag, 0, dataCache);
                            time += L1_CACHE_WRITE_TIME;
                            fprintf(outputFile, "Driver: Write to L1 Cache successfully completed.\n");
                        }
                    }

					else if (retValue == ERROR_WRITE_FAILED_NO_PERMISSION)
                    {
                        fprintf(outputFile, "Driver: Error! write permission not for this memory address\n")
                    }

					else if (retValue == ERROR_CANNOT_WRITE_IN_INSTR_CACHE)
                    {
                        fprintf(outputFile, "Driver: Error! write permission not for this memory address\n")
                    }
					//-------------------------------
				}
				else
				{
					time += L1_CACHE_WRITE_TIME;
					fprintf(outputFile, "Driver: Write to L1 Cache successfully completed.\n");
					continue;
				}
			}            
            else
            {
                //if instruction is read
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
                    int status = readFromMainMem(physicalAddr);
                    if(status == -1)
                    {
                        fprintf(outputFile, "Driver: Error reading From Main Memory\n");
                        continue;
                    }
                    else
                    {
                        fprintf(outputFile, "Driver: Data read from Main Memory\n");
                    }

					//----------------------------
                    updateL2Cache(l2CacheIndex, l2CacheTag, write, 0, dataCache);
                    // Update the LRU in L2 cache

					time += L2_CACHE_UPDATE_TIME;
					fprintf(outputFile, "Driver: L2 Cache update time: %d\n", L2_CACHE_UPDATE_TIME);


                    updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
                    // Update the LRU in L1 cache

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
                    updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
					time += L1_CACHE_UPDATE_TIME;

					// Update the LRU in L1 cache if required.
					// ----------------------------

					fprintf(outputFile, "Driver: L1 Cache update time: %d\n", L1_CACHE_UPDATE_TIME);

			    }
			} // End of that memory access.

			// Updating the time taken by the previous memory access.
			currentTime += time;
			pcb[i].runTime += time;
            ++current_time;

    } 
    
    TLBL1flush();
    TLBL2flush();

    LFUAging();
    fprintf(outputFile, "\n\n\nSIMULATION COMPLETED. ALL PROCESSES FINISHED EXECUTION.\n\n\n");
    for(int i = 0; i < n; ++i)
        fclose(inputFile[i]);
}

