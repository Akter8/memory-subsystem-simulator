#include <stdio.h>
#include <stdlib.h>
#include "dataTypes.h"
#include "utility.h"
#include "configuration.h"
#include "tlb.h"
#include "cache.h"
#include "pageTable.h"
#include "frameTable.h"
#include "segmentTable.h"
#include "pcb.h"
#include "math.h"
#include <string.h>

extern FILE* outputFile;
extern segmentTableInfo* GDTptr;
extern long long current_time;
extern PCB pcbArr[30];
extern void createSegmentFiles(char inputFileName[][100], int n);


void fileNotNull(FILE *file, char *fileName)
{
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: File %s unable to open.\n", fileName);
        exit(0);
    }
}

int fileOpenError(char* fileName)
{
    printf("Error opening the file: %s\n", fileName);
    exit(0);
}

int error(char* str)
{
    printf("%s\n", str);
}


/*
 * Reads the linear address data from the input file.
 */
int readAddr(FILE *fp)
{
    int addr;
    if(fscanf(fp,"%x",&addr)!=EOF)
        return addr;
    else{
       // printf("eof\n");
        return -1;
    }

}

/*
 * Reads the segment number data from the other input file (That will be created in the driver function).
 
int4 readSegNum(FILE *fp, char *write)
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
*/

// Obtains FileNameInputs from input.txt and also creates separate files for segmentNumbers, corresponding to each reference
void ObtainFileNameInput(FILE* input, int n, char SegAddrInputFileName[][100], char LinearAddrInputFileName[][100])
{
    for(int i = 0; i < n; ++i)
    {
        fscanf(input, "%s", LinearAddrInputFileName[i]); 
    }
    fprintf(outputFile, "Driver: Linear adrress input file names received\n");

    // Create the segment number input files for the corresponding linear address files.
    createSegmentFiles(LinearAddrInputFileName, n);

    // Segment number input files will have the same names, but with a prefix of "Segment_".
    for (int i = 0; i < n; ++i)
    {
        strcpy(SegAddrInputFileName[i], "Segment_");
        strcat(SegAddrInputFileName[i], LinearAddrInputFileName[i]);
    }
    fprintf(outputFile, "Driver: Segment number input file names received\n");

}

void InitializationOfHardwareAndTables()
{
    //Flushing the TLBs initially
    TLBL1Flush();
    TLBL2Flush();
    fprintf(outputFile, "Driver: TLBs flushed\n");


    //Initializing Cache
    initL1Cache();
    fprintf(outputFile, "Driver: L1 cache initialized\n");
    initL2Cache();
    fprintf(outputFile, "Driver: L2 cache initialized\n");


    //Initialize Frame Table
    initFrameTable();
    fprintf(outputFile, "Driver: Frame table initialized\n");


    //Global descriptor Table initialize.
    // GDT is only one table.
    GDTptr = initGDTable();
    fprintf(outputFile, "Driver: GDT initialized\n");
}

void EnqueueProcesses(int n, char LinearAddrInputFileName[][100], char SegAddrInputFileName[][100])
{
    //Initializing PCBs of all Processes
    //Initialize segment Table for each Process
    for(int i = 0; i < n; ++i)
    {
        //Initializes PCB
        // LDT for every process is initalised.
        initPCB(i, LinearAddrInputFileName[i], SegAddrInputFileName[i]);
    }
    fprintf(outputFile, "Driver: PCBs initialized\n");    
    fflush(outputFile);
}
int preExecutionWork(int i, int firstExecution[], char* LinearAddrInputFileName, char* SegAddrInputFileName)
{
            // Changes the status of the process
            if(getState(pcbArr[i]) == TERMINATED)
            {
                ++current_time;
                return -1;
            }

            // Checking if this is the first execution of the process to do the prepagin of 2 pages for every process.
            if(firstExecution[i] == 0)
            {
                // Prepages two pages for every process.
                prepaging(i, LinearAddrInputFileName, SegAddrInputFileName);
                firstExecution[i] = 1;
            }

            // Since we follow a very basic (FIFO) scheduler.
            // We do not check for WAITING state as we assume that
            // IO will be finished before the process gets its chance in the processor again.
            setState(&pcbArr[i], RUNNING);
            ++current_time;
            fprintf(outputFile, "\n\n\nDriver: Process-%d running in the processor.\n", i);
            return 0;
    
}
int findPhysicalAddr(int i, int readWrite, int inputAddr, int* error, int4 segNum, long* time)
{
 
    //fprintf(outputFile, "%x %d %c\n",inputAddr,segNum.value,write);

    // First we find the correct mapping of Logical Address to Physical Address.

    // Split the inputAddr to find the requested pageNum.
    int requestedPageOffset = inputAddr & 1023; // (pow(2, 10) - 1), Since page size is 2**10B.
    int requestedPageNum = inputAddr >> 10; // Since we need to discard the least significant 10b.

    fprintf(outputFile, "Driver: Requested PageNumber-%x  PageOffset-%x\n",requestedPageNum,requestedPageOffset);


    // Search this pageNum in both levels of TLB.
    fprintf(outputFile, "Driver: Memory access for %x requested by process-%d\n", inputAddr, i);
    fflush(outputFile);
    int frameNum = TLBL1Search(requestedPageNum,error);
    //int frameNum = TLBL1Search(requestedPageNum,&error);

    *time += L1_TLB_SEARCH_TIME;
    fprintf(outputFile, "Driver: Searched through L1 TLB. Time cost: %d Frame#%d\n", L1_TLB_SEARCH_TIME,frameNum);

    // If the search was unsuccessful in L1 TLB.
    if (frameNum < 0)
    {
        *time += L2_TLB_SEARCH_TIME;
        fprintf(outputFile, "Driver: Did not find required data in L1 TLB. Searched through L2 TLB. Time cost: %d\n", L2_TLB_SEARCH_TIME);
        fflush(outputFile);
        frameNum = TLBL2Search(requestedPageNum,error);

        if (frameNum < 0) // If the data is not present in TLBL2 also.
        {
            // Get the data from MM.
            unsigned int* pageFaultPageNumber=calloc(1, sizeof(unsigned int));
            unsigned int* level=calloc(1, sizeof(unsigned int));
            
            pageTable* pagetable;
            pageTable** ptrToPageFaultPageTable = malloc(sizeof(pageTable *));

            pagetable = searchSegmentTable(i, segNum);

            frameNum = searchPageTable(pagetable, ptrToPageFaultPageTable, inputAddr,readWrite,pageFaultPageNumber,level);

            // If the frame table retrieval resulted in errors.
            if(frameNum == -2)
            {
                //Invalid Address
                fprintf(outputFile, "Driver: InvalidAddress\n");
                current_time += *time;
                pcbArr[i].runTime += *time;
                ++current_time;
                return -2;
            }
            // If the frame table retrieval resulted in errors.
            if(frameNum == -1)
            {

                fprintf(outputFile, "Driver: Page fault has occured: level = %d, pageFaultPageNumber = %d\n", *level, *pageFaultPageNumber);
                fflush(outputFile);

                allocateFrame(i,segNum.value,*ptrToPageFaultPageTable, *pageFaultPageNumber, *level);

                // To go back in the input files so as to be able to run the same instruction again.
                fseek(pcbArr[i].LinearAddrInputFile,-9*sizeof(char),SEEK_CUR);
                fseek(pcbArr[i].SegNumAddrFile,-4*sizeof(char),SEEK_CUR);

                current_time += *time;
                pcbArr[i].runTime += *time;
                ++current_time;
                pcbArr[i].numPageFaults++;
                return -1;
             }
            
            // Once we get the frame num and page table and there is no error.
            fprintf(outputFile, "Driver: Searched through PageTable, found the frameNumber for the required memory reference\n");

            *time += MAIN_MEMORY_SEARCH_TIME;
            fflush(outputFile);

            // Updating TLB and then searching it.
            // Update mimics the kernel updating the TLB.
            // Search mimics the user searching the TLB again after the kernel has updated it.
            TLBL2Update(requestedPageNum, frameNum);
            *time += L2_TLB_UPDATE_TIME;
            fprintf(outputFile, "Driver: Updated L2 TLB. Update time: %d\n", L2_TLB_UPDATE_TIME);
            fflush(outputFile);

            frameNum = TLBL2Search(requestedPageNum, error);
            *time += L2_TLB_SEARCH_TIME;
            fprintf(outputFile, "Driver: Re-searched through L2 TLB. Time cost: %d\n", L2_TLB_SEARCH_TIME);


        }
        // Updating TLB and then searching it.
        // Update mimics the kernel updating the TLB.
        // Search mimics the user searching the TLB again after the kernel has updated it.
        TLBL1Update(requestedPageNum, frameNum);
        *time += L1_TLB_UPDATE_TIME;
        fprintf(outputFile, "Driver: Updated L1 TLB. Update time: %d\n", L1_TLB_UPDATE_TIME);
        fflush(outputFile);

        frameNum = TLBL1Search(requestedPageNum,error);
        *time += L1_TLB_SEARCH_TIME;
        fprintf(outputFile, "Driver: Re-searched through L1 TLB. Time cost: %d\n", L1_TLB_SEARCH_TIME);
    }   
    return frameNum;
}

int AccessData(int i, int inputAddr, unsigned int physicalAddr, bool dataCache, unsigned int readWrite, int4 segNum, long* time, int frameNum, char write)
{
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
        *time += L1_CACHE_WRITE_TIME;
        
        if (retValue < 0)
        {
            // Error Codes.
            if (retValue == ERROR_WRITE_FAILED_NO_TAG_MATCH)
            {
                fprintf(outputFile, "Driver: Write to L1 Cache Failed. Tag did not match. Time cost: %d\n", L1_CACHE_WRITE_TIME);
                fflush(outputFile);

                // Now we search through L2 cache.
                retValue = searchL2Cache(l2CacheIndex, l2CacheTag);
                *time += L2_CACHE_SEARCH_TIME;

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

                    *time += MAIN_MEMORY_WRITE_TIME;
                    fprintf(outputFile, "Driver: Write in Main Memory successfull! %d\n",MAIN_MEMORY_WRITE_TIME);


                    if(status == ERROR_WRITE_FAILED_NO_PERMISSION) 
                    {
                        fprintf(outputFile, "Driver: Error! write permission not for this memory address\n");

                        current_time += *time;
                        pcbArr[i].runTime += *time;
                        ++current_time;
                        return -1;
                    }

                    // We update L2.
                    updateL2Cache(l2CacheIndex, l2CacheTag, write, 0);
                    *time += L2_CACHE_UPDATE_TIME;
                    fprintf(outputFile, "Driver: Updated L2 Cache. Time cost: %d\n", L2_CACHE_WRITE_TIME);
                    fflush(outputFile);

                    // Update L1 now.
                    updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
                    *time += L1_CACHE_UPDATE_TIME;
                    fprintf(outputFile, "Driver: Updated L1 Cache. Time cost: %d\n", L1_CACHE_WRITE_TIME);
                    fflush(outputFile);

                    // Write the data onto L1 now.
                    // Write is by process and update is by kernel.
                    writeL1Cache(l1CacheIndex, l1CacheTag, 0, dataCache);
                    *time += L1_CACHE_WRITE_TIME;
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
                    *time += L1_CACHE_UPDATE_TIME;
                    fprintf(outputFile, "Driver: Updated L1 Cache. Time cost: %d\n", L1_CACHE_WRITE_TIME);
                    fflush(outputFile);

                    // Write the data onto L1 now.
                    // Write is by process and update is by kernel.
                    writeL1Cache(l1CacheIndex, l1CacheTag, 0, dataCache);
                    *time += L1_CACHE_WRITE_TIME;
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
            *time += L1_CACHE_WRITE_TIME;
            fprintf(outputFile, "Driver: Write to L1 Cache successfully completed.\n");

            current_time += *time;
            pcbArr[i].runTime += *time;
            ++current_time;
            return -1;
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
            *time += previousActionTime;

            fprintf(outputFile, "Driver: Found the required data in L1 cache.\n");
            fprintf(outputFile, "Driver: Search time taken: %d.\n", previousActionTime);
            fflush(outputFile);
        }
        else if (retValue1 < 0 && retValue2 < 0)
        {
            // Not there in both the levels of cache.

            int previousActionTime = L2_CACHE_SEARCH_TIME;
            *time += previousActionTime;

            fprintf(outputFile, "Driver: Did NOT find the required data in both L1 and L2 cache\n");
            fprintf(outputFile, "Driver: Search time taken: %d.\n", previousActionTime);
            fprintf(outputFile, "Driver: Will update L1 and L2 caches.\n");
            fflush(outputFile);


            // search in the MM and update caches.
            int status = readFromMemory(physicalAddr>>10);
            if(status == -1)
            {
                fprintf(outputFile, "Driver: Error reading From Main Memory\n");
                return -1;
            }
            else
            {
                fprintf(outputFile, "Driver: Data read from Main Memory\n");
            }
            fflush(outputFile);
            
            // Updating L2.
            updateL2Cache(l2CacheIndex, l2CacheTag, write, 0);
            *time += L2_CACHE_UPDATE_TIME;
            fprintf(outputFile, "Driver: L2 Cache update time, updating after data obtained from Main Memory: %d\n", L2_CACHE_UPDATE_TIME);
            fflush(outputFile);

            // Searching L2.
            // Search is by process and update is by kernel.
            searchL2Cache(l2CacheIndex, l2CacheTag);
            *time += L2_CACHE_SEARCH_TIME;
            fprintf(outputFile, "Driver: L2 Cache search time, searching after data obtained from Main Memory: %d\n", L2_CACHE_SEARCH_TIME);
            fflush(outputFile);

            // Updating L1.
            updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
            *time += L1_CACHE_UPDATE_TIME;
            fprintf(outputFile, "Driver: L1 Cache update time, updating after L2 updated and searched: %d\n", L1_CACHE_UPDATE_TIME);
            fflush(outputFile);

            // Searching L1.
            // Search is by process and update is by kernel.
            searchL1Cache(l1CacheIndex, l1CacheTag, dataCache);
            *time += L1_CACHE_SEARCH_TIME;
            fprintf(outputFile, "Driver: L1 Cache search time, searching after L2 updated and searched: %d\n", L1_CACHE_SEARCH_TIME);
            fflush(outputFile);

        }
        else if (retValue1 < 0 && retValue2 >= 0)
        {
            // Present in L2 cache, but not in L1.

            int previousActionTime = L2_CACHE_SEARCH_TIME;
            *time += previousActionTime;
            fprintf(outputFile, "Driver: Did NOT find the required data in L1 cache, but found it in L2 cache.\n");
            fprintf(outputFile, "Driver: Search time taken: %d.\n", previousActionTime);
            fprintf(outputFile, "Driver: Will update L1 Cache.\n");
            fflush(outputFile);

            // Update L1 cache.
            updateL1Cache(l1CacheIndex, l1CacheTag, write, 0, dataCache);
            *time += L1_CACHE_UPDATE_TIME;
            fprintf(outputFile, "Driver: L1 Cache update time: %d\n", L1_CACHE_UPDATE_TIME);

            // Searching L1 cache.
            // Search is by process and update is by kernel.
            searchL1Cache(l1CacheIndex, l1CacheTag, dataCache);
            *time += L1_CACHE_SEARCH_TIME;
            fprintf(outputFile, "Driver: L1 Cache search time: %d\n", L1_CACHE_SEARCH_TIME);
            fflush(outputFile);
        }
    } // End of that memory access.
return 0;
}

void context_change(int i)
{
    if(getState(pcbArr[i])!=TERMINATED)
        setState(&pcbArr[i], READY);

    // Increment the number of context switches for the process.
    pcbArr[i].numContextSwitches++;

    TLBL1Flush();
    TLBL2Flush();

    frameAgeing();
}

void close_all_files(int n)
{

    // Closing all the input files and output file.
    for(int i = 0; i < n; ++i)
    {
        fclose(pcbArr[i].SegNumAddrFile);
        fclose(pcbArr[i].LinearAddrInputFile);
    }

    fclose(outputFile);
}

void printStatistics(int n)
{
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
}
/*
unsigned int readAddr(FILE* inputFile)
{
    //Stores the line read from inputFile
    char temp[9];
    fscanf(inputFile, "%s", temp);
    
    unsigned int Addr = 0;
    unsigned int mult = 1;
    //Converts read line from file, Hexadecimal into decimal
    for(int i = 7; i >= 0; --i)
    {
        if(48 <= temp[i] && temp[i] <= 57)
        {
            Addr = Addr+(mult*(unsigned int)(temp[i]-48));
        }
        else
        {
            Addr = Addr+(mult*(unsigned int)(temp[i]-87));
        }
        mult = mult*16;
    }
    return Addr;
}

int readSegNum(FILE* SegNumInputFile, char* ReadWrite)
{
   char temp[5];
   fscanf(SegNumInputFile, "%s", temp);

    unsigned int Addr = 0;
    unsigned int mult = 1;
    //Converts read line from file, Hexadecimal into decimal
    for(int i = 4; i >= 0; --i)
    {
        if(48 <= temp[i] && temp[i] <= 57)
        {
            Addr = Addr+(mult*(unsigned int)(temp[i]-48));
        }
        else
        {
            Addr = Addr+(mult*(unsigned int)(temp[i]-87));
        }
        mult = mult*2;
    }

    fscanf(SegNumInputFile, "%c", ReadWrite);

    return Addr;
}
*/
