#include<stdio.h>
#include<stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"      
#include "segmentTable.h"
#include "pcb.h"
#include "frameTable.h"

extern PCB pcbArr[1];
extern segmentTableInfo* GDTptr;
extern FILE *outputFile;

segmentTableInfo* initGDTable()
{
    //Get a non-replacable frame
    unsigned int frameNum = getNonReplaceableFrame();

    if(frameNum == -1)
    {
        printf("Cannot get Non-Replacable Frame for Segment Table\n");
        return NULL;
    }

    //Create a segmentTableObj
    segmentTable* segmentTableObj = malloc(sizeof(segmentTable));

    //Specify whether segmentTable GDT/LDT  //datamember to be added in the segmentTable struct
    segmentTableObj->GDT = 1;

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
    return segmentTableInfoObj;
}

segmentTableInfo* initLDTable(int limit[])
{
    //Get a non-replacable frame
    int frameNum = getNonReplaceableFrame();
    if(frameNum == -1)
    {
        fprintf(outputFile, "Cannot get Non-Replacable Frame for Segment Table");
    }

    //Create a segmentTableObj
    segmentTable *segTableptr;
    segTableptr =(segmentTable *) calloc(1,sizeof(segmentTable));
    segmentTableEntry* entries = calloc(8,sizeof(segmentTableEntry));


    //Specify whether segmentTable GDT/LDT  //datamember to be added in the segmentTable struct
    segTableptr->GDT = 0;

    //segmentTableInfo contains pointer to the segment Table and BaseAddress of the segmentTable
    //this struct would be returned by the current function, to update PCB data members
    segmentTableInfo* segmentTableInfoObj = malloc(sizeof(segmentTableInfo)); 

    segmentTableInfoObj->segmentTableObj = segTableptr;
    segmentTableInfoObj->SegTableBaseAddress= frameNum;     //From retured frame, when getting a non-replacable frame
    
    int readWrite = 0;
    //Initialize all 8 segments in segmentTable
    for(int i = 0; i < 8; ++i)
    {
        //Initalize each entry of segment Table

        if(limit[i] == -1)
        {
            segTableptr->entries[i].present = 0;
            continue;
        }

        //Initalize segmentNum 0, becuase it is present in the process, rest are absent
        //Initalize pageTable for this segment
              //All LDT segments can be written into
        //segTableptr->entries[i].baseAddress = PgTableInfoObj->baseAddress;
        
        segTableptr->entries[i].level3PageTableptr = initPageTable(readWrite);
        segTableptr->entries[i].present = 1;
        segTableptr->entries[i].readWrite = 1;
        segTableptr->entries[i].limit = limit[i];   //limit will be input to the initLDTable

        
    }
    return segmentTableInfoObj;
}
void createGDTsegment(int index, int limit)
{
    GDTptr->segmentTableObj->entries[index].present = 1;

    //Initialize PageTable for the GDT
    pageTable* PgTableObj = initPageTable(0);           //All GDT segments are read-only
    // GDTptr->segmentTableObj.entries[index].baseAddress = PgTableObj->baseAddress;
    GDTptr->segmentTableObj->entries[index].level3PageTableptr = PgTableObj;

    GDTptr->segmentTableObj->entries[index].present = 1;
    GDTptr->segmentTableObj->entries[index].readWrite = 0;
    GDTptr->segmentTableObj->entries[index].limit = limit;
}   



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
        if(GDTptr->segmentTableObj->entries[pid].present==1)
            return pcbArr[pid].LDTPointer->segmentTableObj->entries[segNo].level3PageTableptr;   

    }
    else
    {
        //Local Descriptor table
        printf("The address is in a local segment\n");
        if(pcbArr[pid].LDTPointer->segmentTableObj->entries[segNo].present==1){
            return pcbArr[pid].LDTPointer->segmentTableObj->entries[segNo].level3PageTableptr;   
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