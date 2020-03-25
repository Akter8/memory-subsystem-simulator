/*
 * Author: Chinmay, Divya
 * Date: 21/3/20
 *
 * Contains the ADT of the Page Table
 */

#include <stdio.h>
#include "dataTypes.h"
#include "pageTable.h"
#include "frameTable.h"
#include "pcb.h"
#include "configuration.h"

/// 3 level paging required since linear address size is 32 bits and frame size is 1KB(so 13 bit offset) and each page table entry need 32 bits

extern PCB pcbArr[1];		//testing with only one process right now

int initPageTable(pageTable level3PageTable,pageTable level2PageTable, pageTable level1PageTable){
	int i,j;
	for(i = 0; i < NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE; i++){
		for(j = 0; j < NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE; j++){
			level1PageTable.frames[i].entries[j].present = 0;
			level1PageTable.frames[i].entries[j].modified = 0;	
		}
	}

	for(i = 0; i < NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE; i++){
		for(j = 0; j < NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE; j++){
			level2PageTable.frames[i].entries[j].present = 0;
			level2PageTable.frames[i].entries[j].modified = 0;
			level2PageTable.frames[i].entries[j].cachingDisabled = 1;	
		}
	}

	for(i = 0; i < NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE; i++){
		for(j = 0; j < NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE; j++){
			level3PageTable.frames[i].entries[j].present = 0;				//for now, assuming its present in MM
			level3PageTable.frames[i].entries[j].modified = 0;
			level3PageTable.frames[i].entries[j].cachingDisabled = 1;	
		}
	}
	level3PageTable.nextLevelPageTablePointer = &level2PageTable;
	level2PageTable.nextLevelPageTablePointer = &level1PageTable;
	level1PageTable.nextLevelPageTablePointer = NULL;
	return 0;
}

// linear address size is 32 bits out of which 10 bits will be used for offset. So we will index page table using 22 bits

/* 
This function returns frame number for a given linear address.
Returns -1 and the level and frame number for which page fault occured if unsucessful. 
Also updates LFU count in frame table
*/
int searchPageTable(pageTable level3PageTable,pageTable level2PageTable, pageTable level1PageTable,unsigned int linearAddr, unsigned int* pageFaultPageNumber,unsigned int* level){
	// First 6 bits from left used for indexing level 3

	unsigned int level3Index = linearAddr >> 26;

	printf("linear addr:%x\tindex3:%x\n",linearAddr,level3Index);
	
	if (level3Index > 64)
	{
		printf("Error: Invalid address\n"); 			//level 3 page table has only 8 entries
		return -1;
	}

	//To do: Update Lfu count of the first and only frame of page table level 3. This will require segment table implementation

	unsigned int frameNumberOfLevel2PageTable,indexOfLevel2PageTable=level3Index;
	if(level3PageTable.frames[0].entries[level3Index].present == 1){
		frameNumberOfLevel2PageTable = level3PageTable.frames[0].entries[level3Index].frameNumber;
		updateLfuCount(frameNumberOfLevel2PageTable);		
	}
	else{
		printf("Page fault in level 2 page table\n");	
		*level=2;	
		// *pageFaultPageNumber = level3PageTable.frames[0].entries[level3Index].frameNumber;
		// *pageFaultPageNumber = (linearAddr >> 8) & 0x00000011;
		*pageFaultPageNumber = level3Index;

		printf("From inside searchPageTable: level = %d, pageFaultPageNumber = %d\n",*level,*pageFaultPageNumber);
		return -1;
	}

	printf("\nframeNumberOfLevel2PageTable=%d\n",indexOfLevel2PageTable);

	// Assume we got frame number of level 2 page table

	unsigned int level2Index = (linearAddr >> 18) & 0x000000FF;
	printf("\nlevel2index: %d,	frameof page table index: %d\n",level2Index,indexOfLevel2PageTable);
	unsigned int frameNumberOfLevel1PageTable,indexOfLevel1PageTable=linearAddr>>18;
	if(level2PageTable.frames[indexOfLevel2PageTable].entries[level2Index].present == 1){
		frameNumberOfLevel1PageTable = level2PageTable.frames[indexOfLevel2PageTable].entries[level2Index].frameNumber;
		updateLfuCount(frameNumberOfLevel1PageTable);		
	}
	else{
		printf("Page fault in level 1 page table\n");
		*level=1;
		// *pageFaultPageNumber = level2PageTable.frames[frameNumberOfLevel2PageTable].entries[level2Index].frameNumber;
	//	*pageFaultPageNumber = linearAddr & 0x00000011;
		*pageFaultPageNumber = linearAddr >> 18;
		printf("From inside searchPageTable: level = %d, pageFaultPageNumber = %d\n",*level,*pageFaultPageNumber);


		return -1;
	}

	// Assume we got frame number of level 1 page table

	unsigned int level1Index = (linearAddr>>10) & 0x000000FF;
	
	unsigned int frameNumberOfProcess;
	if(level1PageTable.frames[indexOfLevel1PageTable].entries[level1Index].present == 1){
		frameNumberOfProcess = level1PageTable.frames[indexOfLevel1PageTable].entries[level1Index].frameNumber;
		updateLfuCount(frameNumberOfProcess);		
	}
	else{
		printf("Page fault in process\n");
		*level = 0;
		// *pageFaultPageNumber = level1PageTable.frames[frameNumberOfLevel1PageTable].entries[level1Index].frameNumber;
		*pageFaultPageNumber = linearAddr >> 10;
				//Major doubt: is this correct?
		printf("From inside searchPageTable: level = %d, pageFaultPageNumber = %d\n",*level,*pageFaultPageNumber);

		return -1;
	}

	//Return the frame number. The calling function should concatenate this frame number with offset to get physical address
	
	return frameNumberOfProcess;      
			
}

/*
Sets or resets modified bit of a page table entry
*/
int updatePageTableModifiedBit(pageTable pT,unsigned int index, int value){
	/* if(level == 3){
		level3PageTable.frames[0].entries[index].modified = value;
		return 0;
	}
	else if(level == 2){
		unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
		unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
		level2PageTable.frames[pageNumber].entries[entryNumber].modified = value;
		return 0;
	}
	else{
		unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
		unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
		level1PageTable.frames[pageNumber].entries[entryNumber].modified = value;
		return 0;
	} */

	unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	pT.frames[pageNumber].entries[entryNumber].modified = value;
	return 0;
}

/* 
Sets or resets present bit of a page table entry
*/
int updatePageTablePresentBit(pageTable *pT, unsigned int index, int value){
	/* if(level == 3){
		level3PageTable.frames[0].entries[index].present = value;
		return 0;
	}
	else if(level == 2){
		unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
		unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
		level2PageTable.frames[pageNumber].entries[entryNumber].present = value;
		return 0;
	}
	else{
		unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
		unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
		level1PageTable.frames[pageNumber].entries[entryNumber].present = value;
		return 0;
	} */
	unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	pT->frames[pageNumber].entries[entryNumber].present = value;
	printf("Prsent bit of entry# %d in page number %d has been set to %d\n",entryNumber,pageNumber,value);
	return 0;	
}


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

pageTable* getPointertoNextLevelPageTable(pageTable* pTPointer){
	return pTPointer->nextLevelPageTablePointer;		
}


pageTable* getPageTableFromPid(unsigned int pid,unsigned int segNum,unsigned int level){
	// Access the PCB of the process and use pointers to get access to req page table

	//Get PCB
	PCB pcb = pcbArr[pid-1];
	//

	pageTable* pT3Pointer = getLevel3PageTablePointer(pcb);
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

int setFrameNo(pageTable *pT, unsigned int index, int value)
{
	unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	pT->frames[pageNumber].entries[entryNumber].frameNumber = value;
}

// int setframeOfNextLevel(pageTable *pT, unsigned int index, void *value)
// {
// 	unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
// 	unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
// 	pT->frames[pageNumber].entries[entryNumber].frameOfNextLevel = value;
// }
int setFrameNoOfNextLevel(pageTable *pT, unsigned int index, int value)
{
	unsigned int pageNumber = index / NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	unsigned int entryNumber = index % NUMBER_OF_ENTRIES_PER_PAGE_IN_PAGE_TABLE;
	pT->frames[pageNumber].entries[entryNumber].frameNumberOfNextLevel = value;
}