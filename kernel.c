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

                unsigned int error;
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

                int status = AccessData(i, inputAddr, physicalAddr, dataCache, readWrite, segNum, &time, frameNum, write);
                if(status == -1)
                    continue;

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
