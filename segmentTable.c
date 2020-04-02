#include "segmentTable.h"

segmentTableObj* initGDTable()
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
    segmentTableObj->GDT = 0;

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
    
    GDT->entries[index].present = 1;

    //Initialize PageTable for the GDT
    PageTableInfo* PgTableInfoObj = initPageTable(0);           //All GDT segments are read-only
    GDT[i]->entries[i].baseAddress = PgTableInfoObj->baseAddress;
    GDT[i]->entries[i].Level3PageTable = PgTableInfoObj->pageTableObj;

    GDT[i]->entries[i].present = 1;
    GDT[i]->entries[i].readWrite = 0;
    GDT[i]->entreis[i].limit = limit;
}

