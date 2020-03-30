#include "pageTable.h"

PageTableInfo* initPageTable()
{
    //Get a non-replacable-frame for outermost PageTable
    int19 frameNum = getNonReplacableFrame();

    pageTable* level3PageTable = malloc(sizeof(pageTable));
    level3PageTable->frames = malloc(sizeof(frameOfPageTable));

    pageTable* level2PageTable = malloc(sizeof(pageTable));
    level2PageTable->frames = malloc(sizeof(frameOfPageTable)*256);

    pageTable* level1PageTable = malloc(sizeof(pageTable));
    level1PageTable->frames = malloc(sizeof(frameOfPageTable)*256*256);

    //Filling the pageTable entries for each level of PageTable
    createPageTable(level3PageTable, level2PageTable, level1PageTable);
     
    //Filling PageTableInfo struct, to be returned by this function
    PageTableInfo* PageTableInfoObj = malloc(sizeof(PageTableInfo));
    PageTableInfoObj->pageTableObj = level3PageTable;
    PageTableInfoObj->baseAddress = frameNum;
    
    return PageTableInfoObj;
}
