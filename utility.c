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
