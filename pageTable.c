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
#include "configuration.h"

/// 3 level paging required since linear address size is 32 bits and frame size is 1KB(so 13 bit offset) and each page table entry need 32 bits

	extern pageTable level1PageTable;
	extern pageTable level2PageTable;
	extern pageTable level3PageTable;

// frameOfPageTable level1PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_3_PAGE_TABLE];
// frameOfPageTable level2PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_2_PAGE_TABLE];
// frameOfPageTable level3PageTableFrames[NUMBER_OF_PAGES_IN_LEVEL_1_PAGE_TABLE];

// level1PageTable.frames = level1PageTableFrames;
// level2PageTable.frames = level2PageTableFrames;
// level3PageTable.frames = level3PageTableFrames;


int initPageTable(){
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
			level3PageTable.frames[i].entries[j].present = 0;
			level3PageTable.frames[i].entries[j].modified = 0;
			level3PageTable.frames[i].entries[j].cachingDisabled = 1;	
		}
	}
	return 0;
}

// linear address size is 32 bits out of which 13 bits will be used for offset. So we will index page table using 19 bits

// This function returns frame number for a given linear address or -1 if page fault occurs. Also updates LFU count in frame table
int searchPageTable(unsigned int linearAddr, unsigned int* pageFaultPageNumber,unsigned int *level){
	// Last 3 bits used for indexing level 3

	unsigned int level3Index = linearAddr >> 16;
	
	if (level3Index > 64)
	{
		printf("Error: Invalid address\n"); 			//level 3 page table has only 8 entries
		return -1;
	}

	//To do: Update Lfu count of the first and only frame of page table level 3. This will require segment table implementation

	unsigned int frameNumberOfLevel2PageTable;
	if(level3PageTable.frames[0].entries[level3Index].present == 1){
		frameNumberOfLevel2PageTable = level3PageTable.frames[0].entries[level3Index].frameNumber;
		updateLfuCount(frameNumberOfLevel2PageTable);		
	}
	else{
		printf("Page fault in level 2 page table\n");	
		*level=2;	//Fetch page of level 2 page table??
		*pageFaultPageNumber = level3PageTable.frames[0].entries[level3Index].frameNumber;
		return -1;
	}

	// Assume we got frame number of level 2 page table

	unsigned int level2Index = (linearAddr >> 8) & 0x00000011;
	
	unsigned int frameNumberOfLevel1PageTable;
	if(level2PageTable.frames[frameNumberOfLevel2PageTable].entries[level2Index].present == 1){
		frameNumberOfLevel1PageTable = level2PageTable.frames[frameNumberOfLevel2PageTable].entries[level2Index].frameNumber;
		updateLfuCount(frameNumberOfLevel1PageTable);		
	}
	else{
		printf("Page fault in level 1 page table\n");
		*level=1;
		*pageFaultPageNumber = level2PageTable.frames[frameNumberOfLevel2PageTable].entries[level2Index].frameNumber;
		return -1;
	}

	// Assume we got frame number of level 1 page table

	unsigned int level1Index = linearAddr & 0x00000011;
	
	unsigned int frameNumberOfProcess;
	if(level1PageTable.frames[frameNumberOfLevel1PageTable].entries[level1Index].present == 1){
		frameNumberOfProcess = level1PageTable.frames[frameNumberOfLevel1PageTable].entries[level1Index].frameNumber;
		updateLfuCount(frameNumberOfProcess);		
	}
	else{
		printf("Page fault in process\n");
		*pageFaultPageNumber = level1PageTable.frames[frameNumberOfLevel1PageTable].entries[level1Index].frameNumber;
		return -1;
	}

	//Return the frame number. The calling function should concatenate this frame number with offset to get physical address
	
	return frameNumberOfProcess;      
			
}

/*
Sets or resets modified bit of a page table entry
*/
int updatePageTableModifiedBit(int pid,int segNo,unsigned int index, int value, int level){
	if(level == 3){
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
	}
}

/* 
Sets or resets present bit of a page table entry
*/
int updatePageTablePresentBit(int index, int value, int level){
	if(level == 3){
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
	}
}


// Deletes page table and deallocates frames being used by it
int deallocateProcessPages(){
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

