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

extern PCB pcbObj[30];
extern segmentTable* GDT;
extern frameTable;

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
   
    
    //Creating TLB objects L1 and L2
    TLBL1 tlbL1Obj;
    TLBL2 tlbL2Obj;

    //Flushing the TLBs initially
    TLBL1Flush();
    TLBL2Flush();


    //Creating Cache objects, L1 and L2
    CacheL1 cacheL1obj;
    CacheL2 cacheL2obj;

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
            int4 segNum = readSegNum(PCB[i].SegNumInputFile[i], SegNum);
             
            if(inputAddr == -1)
            {
                setState(PCB[i], TERMINATED);    
                --numProcessAlive;
            }


            //First we find the correct mapping of Logical Address to Physical Address

            // Split the inputAddr to find the requested pageNum.
			int requestedPageOffset = inputAddr & 1023; // (pow(2, 10) - 1), Since page size is 2**10B.
			int requestedPageNum = inputAddr >> 10; // Since we need to discard the least significant 10b.


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

				if (frameNum < 0) // If the data is not present in TLBL2 also.
				{
					// Get the data from MM.

					// --------------------------------

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

            // Based on the MSB of the segNum, decide which cache to hit (data or instr).
			bool dataCache = (segNum[3] == 0) ? true : false;

			// Split the frameNum into index, tag and offset according to both levels of cache's size.
			l1CacheOffset = frameNum & (pow(2, 5) - 1);
			l1CacheIndex = (frameNum & (pow(2, 10) - 1)) >> 5;
			l1CacheTag = frameNum >> 10;

			l2CacheOffset = frameNum & (pow(2, 6) - 1);
			l1CacheIndex = (frameNum & (pow(2, 12) - 1)) >> 6;
			l1CacheTag = frameNum >> 12;


            


            current_time += time;
            ++current_time;
    }
    
    fprintf(outputFile, "\n\n\nSIMULATION COMPLETED. ALL PROCESSES FINISHED EXECUTION.\n\n\n");
    for(int i = 0; i < n; ++i)
        fclose(inputFile[i]);
}

