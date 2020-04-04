#define RUNNING 0
#define READY 1
#define WAIT 2
#define TERMINATED 3

typedef struct
{
	//int29 LDTBaseAddress;
    segmentTableInfo* LDTPointer;
    int pid;	
	int state;
	FILE *LinearAddrInputFile;
	FILE *SegNumAddrFile;
    int4 GDTindex;
    long long swapStartTime;    // Time at which swapping starts when page fault occurs.
    long long runTime;          // Amount of time the process has spent executing in CPU.
    int numContextSwitches;
    int numPageFaults;
}
PCB;

//Initializes ADT of PCB and calls method to initialize LDT of the process
void initPCB(int pid, char* LinearAddrInputFileName, char* segInputFileName);

//Returns pid of the process
int getpid(PCB pcbObj);

//returns state of process
int getState(PCB pcbObj);

//Takes ADT of PCB and state as input and sets the process state 
int setState(PCB* pcbObj, int state);

//Returns reference to level 3 page table given pid and segment number
pageTable* getLevel3PageTablePointer(PCB pcbObj,int segNum);

//Deallocates Process pages and page table 
int deleteProcess(unsigned int pid);

//
void findSegmentLimits(int* gdtSeg_limit, int* ldtSeg_limit, FILE* LinearAddrInputFile, FILE* SegNumAddrInputFile);
