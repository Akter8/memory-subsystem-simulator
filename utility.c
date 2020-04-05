#include "utility.h"
#include "configuration.h"
#include "dataTypes.h"
#include <stdio.h>
#include <stdlib.h>
#include "tlb.h"
#include "cache.h"
#include "pageTable.h"
#include "frameTable.h"
#include "segmentTable.h"
#include "pcb.h"

extern FILE* outputFile;
extern segmentTableInfo* GDTptr;
extern long long current_time;
extern PCB pcbArr[30];


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

    fprintf(outputFile, "%x  %x\n",requestedPageNum,requestedPageOffset);


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
            fprintf(outputFile, "Driver: Did not find required Addr mapping in L2 TLB. Searched through MM PageTable. Time cost: %d\n", MAIN_MEMORY_SEARCH_TIME);
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
