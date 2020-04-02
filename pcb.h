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
    long long swapStartTime;    //Time at which swapping starts when page fault occurs
    long long runTime;          //Amount of time the process has spent executing in CPU
}
PCB;


void initPCB(int pid, char* LinearAddrInputFileName, char* segInputFileName);

//int initPCB(PCB* pcbObj,char *LinearAddrInputFileName,char *SegAddrInputFileName);
int getState(PCB pcbObj);
int setState(PCB* pcbObj, int state);
pageTable* getLevel3PageTablePointer(PCB pcbObj,int segNum);
int26 getLDTBaseAddress(PCB pcbObj);
int deleteProcess(unsigned int pid);