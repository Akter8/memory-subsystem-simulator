#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "cache.h"
#include "dataTypes.h"
#include "pageTable.h"
#include "frameTable.h"
#include "segmentTable.h"
#include "pcb.h"
#include "tlb.h"
//#include "utility.h"

// How many memory accesses before a process if forced to context switch.
#define NUM_LINES_BEFORE_CONTEXT_SWITCH 200


int4
readSegNum(FILE *fp, char *write)
{
    int4 val;
   int x;
    if(fscanf(fp,"%x %c",&x,write)!=EOF){
        val.value = x;
        return val;
    }
    else{
       // printf("eof\n");
        val.value=-1;
        return val;
    }
}

// Global variables.
FILE *outputFile;

PCB pcbArr[30];
segmentTableInfo* GDTptr;

//TLB objects of L1 and L2
TLBL1 l1TLB;
TLBL2 l2TLB;

//Cache objects L1 and L2
CacheL1 l1Cache[2];   //L1 instruction and data cache
CacheL2 l2Cache;

FrameTable frameTable;

long long current_time;


/*
 * The function that runs the entire subsytem.
 */
void
driver()
{
    int n;  // Number of Processes

    FILE* input = fopen("input.txt", "r");
    fscanf(input, "%d", &n);

    //Opening outputFile
    outputFile = fopen(OUTPUT_FILE_NAME, "w");
    fflush(outputFile);

    int error;

    // Taking in input for linear address and segment numbers.
    char SegAddrInputFileName[n][100];
    char LinearAddrInputFileName[n][100];

    //Obtains FileNameInputs from input.txt and also creates separate files for segmentNumbers, corresponding to each reference
    ObtainFileNameInput(input, n, SegAddrInputFileName, LinearAddrInputFileName); 

    fclose(input);

    
    //Initalize all Hardwares-TLB and Cache, Also initialize GDT, FrameTable
    InitializationOfHardwareAndTables();

    
    // Enqueues all processes in READY queue and initializes the corresponding PCB's
    EnqueueProcesses(n, LinearAddrInputFileName, SegAddrInputFileName);


    int firstExecution[30] = {0};
    
    current_time = 0;
    int numProcessAlive = n;
    while(numProcessAlive)
    {
        for(int i = 0; i < n; ++i) // Iterates over the various processses to run them.
        {
            
            int status = preExecutionWork(i, firstExecution, LinearAddrInputFileName[i], SegAddrInputFileName[i]);
            if(status == -1)
                continue;

//            printf("Hello!\n");

            for(int j = 0; j < NUM_LINES_BEFORE_CONTEXT_SWITCH; ++j) // Iterates over various memory accesses / inputs of the same process.
            {
                // Time initialized to zero after every context switch.
                long time = 0;

                // Reads the Memory reference from input file.
                int inputAddr = readAddr(pcbArr[i].LinearAddrInputFile);
            
                char write;
                int4 segNum = readSegNum(pcbArr[i].SegNumAddrFile, &write);
                
                unsigned int readWrite;
                if(write == 'w'){
                    readWrite = 1;
                }
                else{
                    readWrite = 0;
                }

                // If the file has been completely read.
                if(inputAddr == -1)
                {
                    setState(&pcbArr[i], TERMINATED);
                    deleteProcess(i);
                    fprintf(outputFile, "Process %d is terminated\n\n", i);
                    --numProcessAlive;
                    break;
                }

                //Finds in TLB, if miss, then finds in pageTable
                int frameNum = findPhysicalAddr(i, readWrite, inputAddr, &error, segNum, &time);
                if(frameNum == -2)
                    continue;
                else if(frameNum == -1)
                    break;


                // Based on the MSB of the segNum, decide which cache to hit (data or instr).
                bool dataCache = (segNum.value == 0) ? true : false;

                //Physical Addr (Frame Number) obtained. Now search for data
                unsigned int physicalAddr = (frameNum << 10) | (inputAddr & 0x3FF);


                // Split the frameNum into index, tag and offset according to both levels of cache's size.

                // Cache split:  |------16bits---Tag---|--5bits--setNum-|--5bits--offset-|
                unsigned int l1CacheOffset = physicalAddr & 0x1F;
                unsigned int l1CacheIndex = (physicalAddr & 0x3FF) >> 5;
                unsigned int l1CacheTag = physicalAddr >> 10;

                
                // Cache split:  |------14bits--Tag--|--6bits--setNum--|--6bits--offset--|
                unsigned int l2CacheOffset = physicalAddr & ((unsigned int)pow(2, 6) - 1);
                unsigned int l2CacheIndex = (physicalAddr & ((unsigned int)pow(2, 12) - 1)) >> 6;
                unsigned int l2CacheTag = physicalAddr >> 12;

                int retValue;
                if (write == 'w')
                {
                    // Since L1 cache is write through, the driver function only writes to the L1 cache.
                    // And the writeL1Cache() calls writeL2Cache().
                    retValue = writeL1Cache(l1CacheIndex, l1CacheTag, 0, dataCache);
                    time += L1_CACHE_WRITE_TIME;
                    
                    if (retValue < 0)
                    {
                        // Error Codes.
                        if (retValue == ERROR_WRITE_FAILED_NO_TAG_MATCH)
                        {
                            fprintf(outputFile, "Driver: Write to L1 Cache Failed. Tag did not match. Time cost: %d\n", L1_CACHE_WRITE_TIME);
                            fflush(outputFile);

                            // Now we search through L2 cache.
                            retValue = searchL2Cache(l2CacheIndex, l2CacheTag);
                            time += L2_CACHE_SEARCH_TIME;

                            if(retValue < 0)
                            {
                                // Update L2 cache as well since its not there in L2 also.
                                fprintf(outputFile, "Driver: Search in L2 failed! Time cost: %d\n", L2_CACHE_SEARCH_TIME);
                                fflush(outputFile);

                                // Write to Main Memory
                                int status = writeToMemory(physicalAddr>>10);
                                //Update Page Table Dirty Bit
                                pageTable* pT = getPageTableFromPid(i,0,1);
                                unsigned int indexOfLevel1PageTable = inputAddr>>18;
                                unsigned int level1Index = (inputAddr>>10) & 0x000000FF;
                                pT->frames[indexOfLevel1PageTable].entries[level1Index].modified = 1;
                                //

                                time += MAIN_MEMORY_WRITE_TIME;
                                fprintf(outputFile, "Driver: Write in Main Memory successfull! %d\n",MAIN_MEMORY_WRITE_TIME);


                                if(status == ERROR_WRITE_FAILED_NO_PERMISSION) 
                                {
                                    fprintf(outputFile, "Driver: Error! write permission not for this memory address\n");

                                    current_time += time;
                                    pcbArr[i].runTime += time;
                                    ++current_time;
                                    continue;
                                }

                                // We update L2.
                                updateL2Cache(l2CacheIndex, l2CacheTag, write, 0);
                                time += L2_CACHE_UPDATE_TIME;
                                fprintf(outputFile, "Driver: Updated L2 Cache. Time cost: %d\n", L2_CACHE_WRITE_TIME);
                                fflush(outputFile);

                                // Update L1 now.
                                updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
                                time += L1_CACHE_UPDATE_TIME;
                                fprintf(outputFile, "Driver: Updated L1 Cache. Time cost: %d\n", L1_CACHE_WRITE_TIME);
                                fflush(outputFile);

                                // Write the data onto L1 now.
                                // Write is by process and update is by kernel.
                                writeL1Cache(l1CacheIndex, l1CacheTag, 0, dataCache);
                                time += L1_CACHE_WRITE_TIME;
                                fprintf(outputFile, "Driver: Write to L1 Cache successfully completed.\n");
                                fflush(outputFile);
                            }
                            else
                            {
                                // No update in L2 required.
                                fprintf(outputFile, "Driver: Search in L2 successfull! Search Time: %d\n", L2_CACHE_SEARCH_TIME);
                                fflush(outputFile);

                                // L1 updating.
                                updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
                                time += L1_CACHE_UPDATE_TIME;
                                fprintf(outputFile, "Driver: Updated L1 Cache. Time cost: %d\n", L1_CACHE_WRITE_TIME);
                                fflush(outputFile);

                                // Write the data onto L1 now.
                                // Write is by process and update is by kernel.
                                writeL1Cache(l1CacheIndex, l1CacheTag, 0, dataCache);
                                time += L1_CACHE_WRITE_TIME;
                                fprintf(outputFile, "Driver: Write to L1 Cache successfully completed.\n");
                                fflush(outputFile);
                            }
                        }
                        else if (retValue == ERROR_WRITE_FAILED_NO_PERMISSION)
                        {
                            fprintf(outputFile, "Driver: Error! write permission not for this memory address\n");
                        }
                        else if (retValue == ERROR_CANNOT_WRITE_IN_INSTR_CACHE)
                        {
                            fprintf(outputFile, "Driver: Error! write permission not for this memory address\n");
                        }
                    }
                    else // If write was successful in L1 in the first time.
                    {
                        time += L1_CACHE_WRITE_TIME;
                        fprintf(outputFile, "Driver: Write to L1 Cache successfully completed.\n");

                        current_time += time;
                        pcbArr[i].runTime += time;
                        ++current_time;
                        continue;
                    }
                }            
                else
                {
                    // If instruction is read.
                    // Since L1 cache is look aside, we essentially search L1 and L2 cache simultaneously.
                    // But this cannot be shown in the simulation, hence we consider the time taken to do the search to be MIN() of the two of them.
                    // For look-through we would have added the time they take to search.
                    int retValue1 = searchL1Cache(l1CacheIndex, l1CacheTag, dataCache);

                    int retValue2 = searchL2Cache(l2CacheIndex, l2CacheTag);

                    if (retValue1 >= 0 && retValue2 >= 0) // Present in both L1 and L2.
                    {
                        // Return the data to the processor.
                        //return retValue;

                        int previousActionTime = L1_CACHE_SEARCH_TIME;
                        time += previousActionTime;

                        fprintf(outputFile, "Driver: Found the required data in L1 cache.\n");
                        fprintf(outputFile, "Driver: Search time taken: %d.\n", previousActionTime);
                        fflush(outputFile);
                    }
                    else if (retValue1 < 0 && retValue2 < 0)
                    {
                        // Not there in both the levels of cache.

                        int previousActionTime = L2_CACHE_SEARCH_TIME;
                        time += previousActionTime;

                        fprintf(outputFile, "Driver: Did NOT find the required data in both L1 and L2 cache\n");
                        fprintf(outputFile, "Driver: Search time taken: %d.\n", previousActionTime);
                        fprintf(outputFile, "Driver: Will update L1 and L2 caches.\n");
                        fflush(outputFile);


                        // search in the MM and update caches.
                        int status = readFromMemory(physicalAddr>>10);
                        if(status == -1)
                        {
                            fprintf(outputFile, "Driver: Error reading From Main Memory\n");
                            continue;
                        }
                        else
                        {
                            fprintf(outputFile, "Driver: Data read from Main Memory\n");
                        }
                        fflush(outputFile);
                        
                        // Updating L2.
                        updateL2Cache(l2CacheIndex, l2CacheTag, write, 0);
                        time += L2_CACHE_UPDATE_TIME;
                        fprintf(outputFile, "Driver: L2 Cache update time, updating after data obtained from Main Memory: %d\n", L2_CACHE_UPDATE_TIME);
                        fflush(outputFile);

                        // Searching L2.
                        // Search is by process and update is by kernel.
                        searchL2Cache(l2CacheIndex, l2CacheTag);
                        time += L2_CACHE_SEARCH_TIME;
                        fprintf(outputFile, "Driver: L2 Cache search time, searching after data obtained from Main Memory: %d\n", L2_CACHE_SEARCH_TIME);
                        fflush(outputFile);

                        // Updating L1.
                        updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
                        time += L1_CACHE_UPDATE_TIME;
                        fprintf(outputFile, "Driver: L1 Cache update time, updating after L2 updated and searched: %d\n", L1_CACHE_UPDATE_TIME);
                        fflush(outputFile);

                        // Searching L1.
                        // Search is by process and update is by kernel.
                        searchL1Cache(l1CacheIndex, l1CacheTag, dataCache);
                        time += L1_CACHE_SEARCH_TIME;
                        fprintf(outputFile, "Driver: L1 Cache search time, searching after L2 updated and searched: %d\n", L1_CACHE_SEARCH_TIME);
                        fflush(outputFile);

                    }
                    else if (retValue1 < 0 && retValue2 >= 0)
                    {
                        // Present in L2 cache, but not in L1.

                        int previousActionTime = L2_CACHE_SEARCH_TIME;
                        time += previousActionTime;
                        fprintf(outputFile, "Driver: Did NOT find the required data in L1 cache, but found it in L2 cache.\n");
                        fprintf(outputFile, "Driver: Search time taken: %d.\n", previousActionTime);
                        fprintf(outputFile, "Driver: Will update L1 Cache.\n");
                        fflush(outputFile);

                        // Update L1 cache.
                        updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
                        time += L1_CACHE_UPDATE_TIME;
                        fprintf(outputFile, "Driver: L1 Cache update time: %d\n", L1_CACHE_UPDATE_TIME);

                        // Searching L1 cache.
                        // Search is by process and update is by kernel.
                        searchL1Cache(l1CacheIndex, l1CacheTag, dataCache);
                        time += L1_CACHE_SEARCH_TIME;
                        fprintf(outputFile, "Driver: L1 Cache search time: %d\n", L1_CACHE_SEARCH_TIME);
                        fflush(outputFile);
                    }
                } // End of that memory access.

                // Updating the time taken by the previous memory access.
                current_time += time;
                pcbArr[i].runTime += time;
                ++current_time;

            } // End of that process' quota of memory accesses.

            // If the process is not terminated yet, then we set it to ready.
            if(getState(pcbArr[i])!=TERMINATED)
                setState(&pcbArr[i], READY);

            // Increment the number of context switches for the process.
            pcbArr[i].numContextSwitches++;

            TLBL1Flush();
            TLBL2Flush();

            frameAgeing();
        }
        
    
    } // End of all the processes.

    fprintf(outputFile, "\n\n\nDriver: SIMULATION COMPLETED. ALL PROCESSES FINISHED EXECUTION.\n\n\n");

    // Closing all the input files and output file.
    for(int i = 0; i < n; ++i)
    {
        fclose(pcbArr[i].SegNumAddrFile);
        fclose(pcbArr[i].LinearAddrInputFile);
    }

    fclose(outputFile);


    // Prints the statistics onto the statistics file.
    FILE *statisticsFile = fopen(STATISTICS_FILE_NAME, "w");
    fprintf(statisticsFile, "Statistics of all the processes.\n\n");

    // To print the total time taken by processes.
    fprintf(statisticsFile, "Time taken by each process: \n");
    long long int totalTime = 0;
    for (int i = 0; i < n; ++i)
    {
        fprintf(statisticsFile, "Process-%d: %lld\n", i, pcbArr[i].runTime);  
        totalTime += pcbArr[i].runTime;  
    }
    fprintf(statisticsFile, "Total time taken by all processes combined: %lld\n\n", totalTime);

    // To print the total number of context switches.
    fprintf(statisticsFile, "Number of context switches in every process.\n");
    int totalContextSwitches = 0;
    for (int i = 0; i < n; ++i)
    {
        fprintf(statisticsFile, "Process-%d: %d\n", i, pcbArr[i].numContextSwitches);  
        totalContextSwitches += pcbArr[i].numContextSwitches;  
    }
    fprintf(statisticsFile, "Total number of context switches for all the processes combined: %d\n\n", totalContextSwitches);


    // To print the total number of page faults in every process.
    fprintf(statisticsFile, "Number of page faults in every process.\n");
    int totalPageFaults = 0;
    for (int i = 0; i < n; ++i)
    {
        fprintf(statisticsFile, "Process-%d: %d\n", i, pcbArr[i].numPageFaults);
        totalPageFaults += pcbArr[i].numPageFaults;  
    }
    fprintf(statisticsFile, "Total number of context switches for all the processes combined: %d\n\n", totalPageFaults);



    fclose(statisticsFile);

    return;
}
