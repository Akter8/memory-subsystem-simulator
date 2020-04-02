#include <stdio.h>
#include "configuration.h"
#include "cache.h"
#include <stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "frameTable.h"
#include "segmentTable.h"
#include "pcb.h"
#include "tlb.h"
//#include "utility.h"
#define NUM_LINES_BEFORE_CONTEXT_SWITCH 200

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
FILE *outputFile;

PCB pcbArr[30];
segmentTableInfo* GDTptr;

//TLB objects of L1 and L2
TLBL1 tlbL1Obj;
TLBL2 tlbL2Obj;

//Cache objects L1 and L2
CacheL1 cacheL1obj[2];   //L1 instruction and data cache
CacheL2 cacheL2obj;

FrameTable frameTable;

long long current_time = 0;

int main()
{
    int n;      //Number of processes  
    scanf("%d", &n);

    int error;

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
    GDTptr=initGDTable();

    printf("Initialised");

    //Initializing PCBs of all Processes
    //Initialize segment Table for each Process
    for(int i = 0; i < n; ++i)
    {
        //Initializes PCB
        initPCB(i, LinearAddrInputFileName[i], SegAddrInputFileName[i]);
        //initPCB(&pcbArr[i]);

    }

    //Opening outputFile
    outputFile = fopen(OUTPUT_FILE_NAME, "w");
   // fclose(outputFile);

    int numProcessAlive = n;
while(numProcessAlive)
{
    for(int i = 0; i < n; ++i)
    {
        
        //Changes the status of the process
        if(getState(pcbArr[i]) == TERMINATED)
        {
            ++current_time;
            continue;
        }
    
        setState(&pcbArr[i], RUNNING);
        ++current_time;
        fprintf(outputFile, "\n\n\nDriver: Process-%d running in the processor.\n", i);
    


        for(int j = 0; j < NUM_LINES_BEFORE_CONTEXT_SWITCH; ++j)
        {
            //time initialized to zero after every context switch
            long time = 0;

            //Reads the Memory reference from input file
            int inputAddr = readAddr(pcbArr[i].LinearAddrInputFile);
        
            char write;
            int4 segNum = readSegNum(pcbArr[i].SegNumAddrFile, &write);
            
            unsigned int readWrite = 0;             //All accesses read to avoid errors for now

            if(inputAddr==-1)
            {
                setState(&pcbArr[i],TERMINATED );
                --numProcessAlive;
                break;
            }
            printf("%x %d %c\n",inputAddr,segNum.value,write);

            //First we find the correct mapping of Logical Address to Physical Address

            // Split the inputAddr to find the requested pageNum.
            int requestedPageOffset = inputAddr & 1023; // (pow(2, 10) - 1), Since page size is 2**10B.
            int requestedPageNum = inputAddr >> 10; // Since we need to discard the least significant 10b.

            printf("%x  %x\n",requestedPageNum,requestedPageOffset);
            // Based on the MSB of the segNum, decide which cache to hit (data or instr).
            bool dataCache = (segNum.value == 0) ? true : false;


            // Search this pageNum in both levels of TLB.
           fprintf(outputFile, "Driver: Memory access for %x requested by process-%d\n", inputAddr, i);
            int frameNum = TLBL1Search(requestedPageNum,&error);

            time += L1_TLB_SEARCH_TIME;
            fprintf(outputFile, "Driver: Searched through L1 TLB. Time cost: %d Frame#%d\n", L1_TLB_SEARCH_TIME,frameNum);

            if (frameNum < 0)
            {
                time += L2_TLB_SEARCH_TIME;
                fprintf(outputFile, "Driver: Did not find required data in L1 TLB. Searched through L2 TLB. Time cost: %d\n", L2_TLB_SEARCH_TIME);
                frameNum = TLBL2Search(requestedPageNum,&error);
                if (frameNum < 0) // If the data is not present in TLBL2 also.
                {
                    // Get the data from MM.
                    unsigned int* pageFaultPageNumber=calloc(1, sizeof(unsigned int));
                    unsigned int* level=calloc(1, sizeof(unsigned int));
                    // if(dataCache)        //if memory reference is data
                    // {   
                    //     frameNum = searchPageTable(PCB[i].segTableObj->entries[segNum], ptrToPageFaultTable, 
                    //                                 inputAddr, readWrite, pageFaultPageNumber, level);
                    // }
                    // else{               //if memory reference is code
                    //     frameNum = searchPageTable(GDT[PCB[i].GDTindex]->pageTableObj, ptrToPageFaultTable, 
                    //                             inputAddr, readWrite, pageFaultPageNumber, level);    
                    // }
                    pageTable* pagetable;
                    pageTable** ptrToPageFaultPageTable = malloc(sizeof(pageTable *));

                    pagetable = searchSegmentTable(i, segNum);

                    frameNum = searchPageTable(pagetable, ptrToPageFaultPageTable, inputAddr,readWrite,pageFaultPageNumber,level);


                    if(frameNum == -2)
                    {
                        //Invalid Address
                        continue;
                    }



                    if(frameNum == -1)
                    {

                        printf("After returing to driver searchPageTable: level = %d, pageFaultPageNumber = %d\n", *level, *pageFaultPageNumber);
        
                        allocateFrame(i,segNum.value,*ptrToPageFaultPageTable, *pageFaultPageNumber, *level);

                        fseek(pcbArr[i].LinearAddrInputFile,-9*sizeof(char),SEEK_CUR);
                        break;
                     }
                    
        
                    fprintf(outputFile, "Driver: Searched through PageTable, found the frameNumber for the required memory reference\n");

                    time += MM_SEARCH_TIME;
                    fprintf(outputFile, "Driver: Did not find required Addr mapping in L2 TLB. Searched through MM PageTable. Time cost: %d\n", MM_SEARCH_TIME);

                    TLBL2Update(requestedPageNum, frameNum);
                    time += L2_TLB_UPDATE_TIME;
                    fprintf(outputFile, "Driver: Updated L2 TLB. Update time: %d\n", L2_TLB_UPDATE_TIME);

                    frameNum = TLBL2Search(requestedPageNum,&error);
                    time += L2_TLB_SEARCH_TIME;
                    fprintf(outputFile, "Driver: Re-searched through L2 TLB. Time cost: %d\n", L2_TLB_SEARCH_TIME);


                }

                TLBL1Update(requestedPageNum, frameNum);
                time += L1_TLB_UPDATE_TIME;
                fprintf(outputFile, "Driver: Updated L1 TLB. Update time: %d\n", L1_TLB_UPDATE_TIME);

                frameNum = TLBL1Search(requestedPageNum,&error);
                time += L1_TLB_SEARCH_TIME;
                fprintf(outputFile, "Driver: Re-searched through L1 TLB. Time cost: %d\n", L1_TLB_SEARCH_TIME);




            }   




        }

    }
   // numProcessesAlive--;
}


fclose(outputFile);
    
}
