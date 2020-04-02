#include<stdio.h>
#include<stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"      
#include "segmentTable.h"
#include "pcb.h"

extern PCB pcbArr[1];
extern segmentTable* GDTptr;


/*segmentTableInfo* initGDTable()
{
    //Get a non-replacable frame
    int19 frameNum = getNonReplacableFrame();

    if(status == -1)
    {
        error("Cannot get Non-Replacable Frame for Segment Table");
    }

    //Create a segmentTableObj
    segmentTable* segmentTableObj = malloc(sizeof(segmentTable));

    //Specify whether segmentTable GDT/LDT  //datamember to be added in the segmentTable struct
    segmentTableObj->GDTptr = 1;

    //segmentTableInfo contains pointer to the segmentTableObj and BaseAddress of the segmentTable
    //this struct would be returned by the current function, to update PCB data members
    segmentTableInfo* segmentTableInfoObj = malloc(sizeof(segmentTableInfo)); 

    segmentTableInfoObj->segmentTableObj = segmentTableObj;
    segmentTableInfoObj->SegTableBaseAddress = frameNum;     //From retured frame, when getting a non-replacable frame
    
    //Initialize all 8 segments in segmentTable
    for(int i = 0; i < 8; ++i)
    {
        segmentTableObj->entries[i].present = 0;
    }
    return segmentTableObj;
}

segmentTableInfo* initLDTable(int limit[])
{
    //Get a non-replacable frame
    int frameNum = getNonReplacableFrame();
    if(frameNum == -1)
    {
        fprintf(outputFile, "Cannot get Non-Replacable Frame for Segment Table");
    }

    //Create a segmentTableObj
    segmentTable* segmentTableObj = malloc(sizeof(segmentTable));

    //Specify whether segmentTable GDT/LDT  //datamember to be added in the segmentTable struct
    segmentTableObj->GDTptr = 0;

    //segmentTableInfo contains pointer to the segmentTableObj and BaseAddress of the segmentTable
    //this struct would be returned by the current function, to update PCB data members
    segmentTableInfo* segmentTableInfoObj = malloc(sizeof(segmentTableInfo)); 

    segmentTableInfoObj->segmentTableObj = segmentTableObj;
    segmentTableInfoObj->SegTableBaseAddress = frameNum;     //From retured frame, when getting a non-replacable frame
    

    //Initialize all 8 segments in segmentTable
    for(int i = 0; i < 8; ++i)
    {
        //Initalize each entry of segment Table

        if(limit[i] == -1)
        {
            segmentTableObj->entries[i].present = 0;
            continue;
        }

        //Initalize segmentNum 0, becuase it is present in the process, rest are absent
        //Initalize pageTable for this segment
        PageTableInfo* PgTableInfoObj = initPageTable(1);       //All LDT segments can be written into
        segmentTableObj->entries[i].baseAddress = PgTableInfoObj->baseAddress;
        segmentTableObj->entries[i].Level3PageTable = PgTableInfoObj->pageTableObj;

        segmentTableObj->entries[i].present = 1;
        segmentTableObj->entries[i].readWrite = 1;
        segmentTableObj->entries[i].limit = limit[i];   //limit will be input to the initLDTable

        free(PgTableInfoObj);
        
    }
}

void createGDTsegment(int index, int limit)
{
    GDTptr->entries[index].present = 1;

    //Initialize PageTable for the GDT
    PageTableInfo* PgTableInfoObj = initPageTable(0);           //All GDT segments are read-only
    GDTptr[i]->entries[i].baseAddress = PgTableInfoObj->baseAddress;
    GDTptr[i]->entries[i].Level3PageTable = PgTableInfoObj->pageTableObj;

    GDTptr[i]->entries[i].present = 1;
    GDTptr[i]->entries[i].readWrite = 0;
    GDTptr[i]->entreis[i].limit = limit;
}   


*/


/*
Takes pid virtual address produced by processor as input 
and returns pointer to level 3 page table of required segment
*/
pageTable* searchSegmentTable(int pid, int4 segNum)
{
    unsigned int segNo = (segNum.value)&0x7;
    unsigned int localGlobal = (segNum.value>>3);

    if(localGlobal==1)
    {
        //Global Descriptor table
        printf("The address is in a global segment\n");
        if(GDTptr->entries[segNo].present==1)
            return pcbArr[pid].LDTPointer->entries[segNo].level3PageTableptr;   

    }
    else
    {
        //Local Descriptor table
        printf("The address is in a local segment\n");
        if(pcbArr[pid].LDTPointer->entries[segNo].present==1){
            return pcbArr[pid].LDTPointer->entries[segNo].level3PageTableptr;   
        }
    }

    return 0;
}




/*
Takes segment table entry ADT and value as input and
updates present bit of that entry to value 
*/              
int updateSegmentTablePresentBit(segmentTableEntry* segTableEntry, int index, int value){

        segTableEntry->present = value;
        return 1;

}   //value = 0 or 1




/*
Initializes ADTs for segment table and calls method that initializes page table of one segment 
*/
segmentTable* initSegTable()
{
    segmentTable *segTableptr;
    segTableptr =(segmentTable *) calloc(1,sizeof(segmentTable));
    segmentTableEntry* entries = calloc(8,sizeof(segmentTableEntry));


    for(int i = 0; i < 8; i++){
        segTableptr->entries[i] = entries[i];
    }

    for(int i=0;i<8;i++)
    {
        segTableptr->entries[i].present=1;
        segTableptr->entries[i].level3PageTableptr  = NULL;
    }

    int readWrite = 1;
    segTableptr->entries[0].level3PageTableptr = initPageTable(readWrite);

    return segTableptr;

}