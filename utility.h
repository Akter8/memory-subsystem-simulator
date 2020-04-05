#include <stdio.h>
//#include <stdbool.h>

// General utility functions to run the program.
int fileOpenError(char* fileName);
int readAddr(FILE* inputFile);
void fileNotNull(FILE *file, char *fileName);

int error(char* str);

//int4 readSegNum(FILE *fp, char *write);

void ObtainFileNameInput(FILE* input, int n, char SegAddrInputFileName[][100], char LinearAddrInputFileName[][100]);

// To create the segment table inputs for the program.
//void createSegmentFiles(char [], int);
int findReadOrWriteMemoryAccess();

// The function that drives the subsytem and called by the main().
void driver();

//Initializes TLBs, Cache, Frame Table and GDT
void InitializationOfHardwareAndTables();

// Enqueus all processes in READY queue, when simulation starts
// All PCBs initialized
void EnqueueProcesses(int n, char LinearAddrInputFileName[][100], char SegAddrInputFileName[][100]);

// Instructions to be run just before a process begins executing, pre-paging is done, process state is changed
int preExecutionWork(int i, int firstExecution[], char* LinearAddrInputFileName, char* SegAddrInputFileName);

//finds Physical Address from logical address, searches in TLB and PageTable to returns the frameNum of the memory reference. If a pageFault occurs, brings the page to memory and returns -1
int findPhysicalAddr(int i, int readWrite, int inputAddr, int* error, int4 segNum, long* time);


// runs all instructions necessary for reading/writing data to Cache and Main Memory. Cache/Main Memory is accessed which might change, entries in Frame Table, Cache etc. If failure to access data due to permission problems, then returns -1
//int AccessData(int i, int inputAddr, unsigned int physicalAddr, bool dataCache, unsigned int readWrite, int4 segNum, long* time, int frameNum, char write)

//When a process, completes its execution time or a pageFault happens the code calls this function. It contains the code necessary to put the present process in waiting and start the next process.
void context_change(int i);


// Closes all open files, the files containg memory references, and the files containg corresponding segNum of the reference. Also closes the output file
void close_all_files(int n);

// Prints a summary of the entire Simulation. For each process prints the Time taken, Number of Page Faults and the number of context-switches
void printStatistics(int n);
