/*
 * Author: Chinmay, Divya
 * Date: 21/3/20
 *
 * Contains the ADT of the Page Table
 */

#include <stdio.h>
#include <stdlib.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "frameTable.h"
#include "segmentTable.h"
#include "pcb.h"
#include "configuration.h"

/*
3 level paging required since linear address size is 32 bits and frame size is 1KB(so 10 bit offset) and  
each page table entry need 32 bits
*/

extern PCB pcbArr[1];		//testing with only one process right now
extern FILE *outputFile;




/*
Initializes all 3 levels of page table by dynamically allocating memory
for ADTs correxponding to page tables and their frames.
Returns a pointer to level 3 page table
*/
pageTable* initPageTable(int readWrite){
	pageTable *level3PageTableptr, *level2PageTableptr, *level1PageTableptr;
	frameOfPageTable *level3PageTableFramesptr,*level2PageTableFramesptr,*level1PageTableFramesptr;

	level1PageTableptr = (pageTable*) calloc(1, sizeof(pageTable));
	level2PageTableptr = (pageTable*) calloc(1, sizeof(pageTable));
	level3PageTableptr = (pageTable*) calloc(1, sizeof(pageTable));

	level3PageTableFramesptr = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE, sizeof(frameOfPageTable));
	level2PageTableFramesptr = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE, sizeof(frameOfPageTable));
	level1PageTableFramesptr = (frameOfPageTable*) calloc(NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE, sizeof(frameOfPageTable));

	level3PageTableptr->frames = level3PageTableFramesptr;
	level2PageTableptr->frames = level2PageTableFramesptr;
	level1PageTableptr->frames = level1PageTableFramesptr;

	int i,j;
	for(i = 0; i < NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE; i++){
		for(j = 0; j < NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE; j++){
			level1PageTableptr->frames[i].entries[j].present = 0;
			level1PageTableptr->frames[i].entries[j].modified = 0;
			// level1PageTableptr->frames[i].entries[j].readWrite = 0;
			if(readWrite == 0){
				//Its a read only segment
				level1PageTableptr->frames[i].entries[j].readWrite = 0;
			}
			else{
				level1PageTableptr->frames[i].entries[j].readWrite = 1;
			}	
		}
	}

	for(i = 0; i < NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE; i++){
		for(j = 0; j < NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE; j++){
			level2PageTableptr->frames[i].entries[j].present = 0;
			level2PageTableptr->frames[i].entries[j].modified = 0;
			level2PageTableptr->frames[i].entries[j].cachingDisabled = 1;
			level2PageTableptr->frames[i].entries[j].readWrite = 1;	
		}
	}

	for(i = 0; i < NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE; i++){
		for(j = 0; j < NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE; j++){
			level3PageTableptr->frames[i].entries[j].present = 0;				
			level3PageTableptr->frames[i].entries[j].modified = 0;
			level3PageTableptr->frames[i].entries[j].cachingDisabled = 1;
			level3PageTableptr->frames[i].entries[j].readWrite = 1;	
		}
	}
	level3PageTableptr->nextLevelPageTablePointer = level2PageTableptr;
	level2PageTableptr->nextLevelPageTablePointer = level1PageTableptr;
	level1PageTableptr->nextLevelPageTablePointer = NULL;
	return level3PageTableptr;
}





/* 
This function returns physical frame number for a given linear address.
Returns -1 and the level of paging and page number for which page fault occured if unsucessful. 
Also updates LFU count in frame table
*/
int searchPageTable(pageTable* level3PageTable,pageTable** ptref,unsigned int linearAddr, unsigned int readWrite,
	unsigned int* pageFaultPageNumber, unsigned int* level){

	// fprintf(outputFile, "Inside searchPageTable\n");
	pageTable* level2PageTable = level3PageTable->nextLevelPageTablePointer;

	
	pageTable* level1PageTable = level2PageTable->nextLevelPageTablePointer;

	// First 6 bits from left used for indexing level 3

	unsigned int level3Index = linearAddr >> 26;

	fprintf(outputFile, "linear addr:%x\tindex3:%x\n",linearAddr,level3Index);
	if (level3Index > 64)
	{
		fprintf(outputFile, "Error: Invalid address\n"); 			//level 3 page table has only 8 entries
		return -2;
	}

	//To do: Update Lfu count of the first and only frame of page table level 3. This will require segment table implementation

	unsigned int frameNumberOfLevel2PageTable,indexOfLevel2PageTable=level3Index;

	if(level3PageTable->frames[0].entries[level3Index].present == 1){
		frameNumberOfLevel2PageTable = level3PageTable->frames[0].entries[level3Index].frameNumber;
		updateLfuCount(frameNumberOfLevel2PageTable);		
	}
	else{
		fprintf(outputFile, "Page fault in level 2 page table\n");	
		*level=2;	
		*pageFaultPageNumber = level3Index;
		*ptref = level3PageTable;
		return -1;
	}

	// We have got frame number of level 2 page table

	unsigned int level2Index = (linearAddr >> 18) & 0x000000FF;
	fprintf(outputFile, "\nlevel2index: %d,	frameof page table index: %d\n",level2Index,indexOfLevel2PageTable);
	unsigned int frameNumberOfLevel1PageTable,indexOfLevel1PageTable=linearAddr>>18;
	
	if(level2PageTable->frames[indexOfLevel2PageTable].entries[level2Index].present == 1){
		frameNumberOfLevel1PageTable = level2PageTable->frames[indexOfLevel2PageTable].entries[level2Index].frameNumber;
		updateLfuCount(frameNumberOfLevel1PageTable);		
	}
	else{
		fprintf(outputFile, "Page fault in level 1 page table\n");
		*level=1;
		*pageFaultPageNumber = linearAddr >> 18;
		*ptref = level2PageTable;

		return -1;
	}

	// We have got frame number of level 1 page table

	unsigned int level1Index = (linearAddr>>10) & 0x000000FF;
	
	unsigned int frameNumberOfProcess;
	if(level1PageTable->frames[indexOfLevel1PageTable].entries[level1Index].present == 1){
		//Checking read/write permissions
		if(level1PageTable->frames[indexOfLevel1PageTable].entries[level1Index].readWrite == 0 && readWrite == 1){
			//Page is read only but we are trying to write
			fprintf(outputFile, "Write permission denied. The page is a read only page.\n");
		}
		
		
		frameNumberOfProcess = level1PageTable->frames[indexOfLevel1PageTable].entries[level1Index].frameNumber;
		
		updateLfuCount(frameNumberOfProcess);		
	}
	else{
		fprintf(outputFile, "Page fault in process\n");
		*level = 0;
		*pageFaultPageNumber = linearAddr >> 10;
		*ptref = level1PageTable;

		return -1;
	}

	//Return the frame number. The calling function should concatenate this frame number with offset to get physical address
	
	return frameNumberOfProcess;      
			
}





/*
Sets or resets modified bit of a page table entry
*/
int updatePageTableModifiedBit(pageTable* pageTableptr,unsigned int index, int value){
	unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	pageTableptr->frames[pageNumber].entries[entryNumber].modified = value;
	fprintf(outputFile, "Modified bit of entry# %d in page number %d has been set to %d\n",entryNumber,pageNumber,value);
	return 0;
}





/* 
Sets or resets present bit of a page table entry
*/
int updatePageTablePresentBit(pageTable *pT, unsigned int index, int value){
	unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	pT->frames[pageNumber].entries[entryNumber].present = value;
	fprintf(outputFile, "Present bit of entry# %d in page number %d has been set to %d\n",entryNumber,pageNumber,value);
	return 0;	
}




/*
Takes a pointer to a pageTable and
returns a pointer to the ADT of the next level pageTable
*/
pageTable* getPointertoNextLevelPageTable(pageTable* pTPointer){
	return pTPointer->nextLevelPageTablePointer;		
}




/*
Takes pid, segment number in LDT and level of page table required as input
and returns a pointer to the required page table 
*/
pageTable* getPageTableFromPid(unsigned int pid,unsigned int segNum,unsigned int level){
	// Access the PCB of the process and use pointers to get access to req page table

	//Get PCB
	PCB pcb = pcbArr[pid];
	//

	pageTable* pT3Pointer = getLevel3PageTablePointer(pcb,segNum);
	pageTable* pT2Pointer;
	pageTable* pT1Pointer;
	if(level == 3){
		return pT3Pointer;
	}
	else if(level == 2){
		pT2Pointer = getPointertoNextLevelPageTable(pT3Pointer);
		return pT2Pointer;
	}
	else if(level == 1){
		pT2Pointer = getPointertoNextLevelPageTable(pT3Pointer);
		pT1Pointer = getPointertoNextLevelPageTable(pT2Pointer);
		return pT1Pointer;
	}
}




/*
Sets the frameNumber field of the entry in the pageTable pointed 
to by pT with specified index to value
*/
int setFrameNo(pageTable *pT, unsigned int index, int value)
{
	unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	pT->frames[pageNumber].entries[entryNumber].frameNumber = value;
	return 0;
}




/*
// Deletes page table and deallocates frames being used by it
int deallocateProcessPages(pageTable level3PageTable, pageTable level2PageTable, pageTable level1PageTable){
	// Mark the frames of the page table as empty in the frame table
	int i,j;
	for(i = 0; i < NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE; i++){
		for(j = 0; j < NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE; j++){
			if(level1PageTable.frames[i].entries[j].present == 1){
				int frameToDelete = level1PageTable.frames[i].entries[j].frameNumber;
				setEmptyBitFrameTable(frameToDelete, 0);
			}	
		}
	}

	for(i = 0; i < NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE; i++){
		for(j = 0; j < NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE; j++){
			if(level2PageTable.frames[i].entries[j].present == 1){
				int frameToDelete = level2PageTable.frames[i].entries[j].frameNumber;
				setEmptyBitFrameTable(frameToDelete, 0);
			}	
		}
	}

	for(i = 0; i < NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE; i++){
		for(j = 0; j < NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE; j++){
			if(level3PageTable.frames[i].entries[j].present == 1){
				int frameToDelete = level3PageTable.frames[i].entries[j].frameNumber;
				setEmptyBitFrameTable(frameToDelete, 0);
			}
		}
	}

	return 0;

}
*/
int findFirst2Pages(char *linearAddrInp,char *segNumInp,int *pageCode,int *pageData)
{
	int pageCodefound=0,pageDatafound=0;

	FILE *fpLinearAddr = fopen(linearAddrInp,"r");
	FILE *fpSegNum = fopen(segNumInp,"r");

	if(fpLinearAddr==NULL||fpSegNum==NULL)
		return -1;

	int addr,segNum;
	char readWrite;

	while(fscanf(fpLinearAddr,"%x",&addr)!=0 && (pageCodefound==0 || pageDatafound==0))
	{
		fscanf(fpSegNum,"%x %c",&segNum,&readWrite);

		//printf("pageno:%x segNum:%d\n",addr>>10,segNum);

		if(segNum==0x8&&pageCodefound==0){
			*pageCode = addr>>10;
			pageCodefound = 1;
		}
		else if(segNum==0x0&&pageDatafound==0){
			*pageData = addr>>10;
			pageDatafound = 1;
		}

		
	}

	printf("\n\npage1:%x \npage2:%x\n",*pageCode,*pageData);
	fclose(fpLinearAddr);
	fclose(fpSegNum);
	return 0;

}

int prepaging(int pid,char *LinearAddrInputFile,char *SegNumAddrFile)
{

	int pageNumCode,pageNumData;

	//find first data page and code page
	if(findFirst2Pages(LinearAddrInputFile,SegNumAddrFile,&pageNumCode,&pageNumData)==-1)
		return -1;


	//get page table frame corresponding to the first code and data pages

	int pt1NumCode = pageNumCode>>8,
		pt2NumCode = pageNumCode>>16,
		pt1NumData = pageNumData>>8,
		pt2NumData = pageNumData>>16;
	int4 segNumCode,segNumData;

	segNumCode.value = 8;
	segNumData.value = 0;

	pageTable *pt3RefCode = searchSegmentTable(pid,segNumCode);
	pageTable *pt3RefData = searchSegmentTable(pid,segNumData);

	//Level 2 page table frame for Code 
	allocateFrame(pid,0x8,pt3RefCode,pt2NumCode,2);	

	//Level 1 page table frame for Code 
	pageTable *pt2RefCode = getPointertoNextLevelPageTable(pt3RefCode);
	allocateFrame(pid,0x8,pt2RefCode,pt1NumCode,1);	


	//Level 2 page table frame for Data
	allocateFrame(pid,0x0,pt3RefData,pt2NumData,2);	

	pageTable *pt2RefData = getPointertoNextLevelPageTable(pt3RefData);
	//Level 1 page table frame for Data 
	allocateFrame(pid,0x0,pt2RefData,pt1NumData,1);	


	//allocate frame for first code page
	allocateFrame(pid,0x8,getPointertoNextLevelPageTable(pt2RefCode),pageNumCode ,0);	

	//allocate frame for first data page
	allocateFrame(pid,0x0,getPointertoNextLevelPageTable(pt2RefData),pageNumData ,0);	


}