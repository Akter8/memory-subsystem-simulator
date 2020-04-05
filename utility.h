#include <stdio.h>

#define INPUT_FILE1_NAME "Inputs/APSI.txt"
#define INPUT_FILE2_NAME "Inputs/CC1.txt"
#define INPUT_FILE3_NAME "Inputs/LI.txt"
#define INPUT_FILE4_NAME "Inputs/M88KSIM.txt"
#define INPUT_FILE5_NAME "Inputs/VORTEX.txt"

#define debug printf("Hello, hi\n")
#define debug_str(x) printf("--> %s", x)
#define debug_int(x) printf("--> %d", x)

// General utility functions to run the program.
int fileOpenError(char* fileName);
int readAddr(FILE* inputFile);
void fileNotNull(FILE *file, char *fileName);

int error(char* str);

//int4 readSegNum(FILE *fp, char *write);

void ObtainFileNameInput(FILE* input, int n, char SegAddrInputFileName[][100], char LinearAddrInputFileName[][100]);

// To create the segment table inputs for the program.
void createSegmentFiles(char [], int);
int findReadOrWriteMemoryAccess();

// The function that drives the subsytem and called by the main().
void driver();

//Initializes TLBs, Cache, Frame Table and GDT
void InitializationOfHardwareAndTables();

//Enqueus all processes in READY queue, when simulation starts
//All PCBs initialized
void EnqueueProcesses(int n, char LinearAddrInputFileName[][100], char SegAddrInputFileName[][100]);

int preExecutionWork(int i, int firstExecution[], char* LinearAddrInputFileName, char* SegAddrInputFileName);
