/*
 * Author: Akhil
 * Date: 22/3/20
 *
 * Based on the linear address, this module generates the address used by the segment table.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>


// What percent of memory accesses are reads.
#define PERCENT_OF_READS 80
#define READ 0
#define WRITE 1

#define NUM_INPUT_FILES 5
 

/*
 * Returns whether a memory access will be a read memory access or a write one.
 * Its based on a psuedo random number generator.
 */
int
findReadOrWriteMemoryAccess()
{
	int num = rand() % 100;
	
	if (num <= PERCENT_OF_READS)
		return READ;
	else
		return WRITE;
}


int main(int argc, char const *argv[])
{

	srand(time(NULL));

	// All the file names.
	char *inputFileName[NUM_INPUT_FILES] = {"APSI.txt", "LI.txt", "CC1.txt", "M88KSIM.txt", "VORTEX.txt"};

	for (int i = 0; i < NUM_INPUT_FILES; ++i) // One file at a time.
	{
		char *inputFilePointer = inputFileName[i];
		char outputFileName[100];

		strcpy(outputFileName, "Segment_");
		strcat(outputFileName, inputFileName[i]);

		FILE *inputFile, *outputFile;
		inputFile = fopen(inputFilePointer, "r");
		outputFile = fopen(outputFileName, "w");

		char inputChar, readWrite;
		int inputInt, readOrWrite;

		while( fscanf(inputFile, "%c%x ", &inputChar, &inputInt) >= 1 )
		{
			// Finding if the memory access was a read or write.
			readOrWrite = findReadOrWriteMemoryAccess();
			readWrite = 'r';
			if (readOrWrite == WRITE)
				readWrite = 'w';

			// Writing into the file.
			if (inputChar == '7')
	   			fprintf(outputFile, "1000 %c\n", 'r'); // Since the inputs starting from 7 are part of the code segment which is read only, we hard code only reads to these memory accesses.
	   		else
	   			fprintf(outputFile, "0000 %c\n", readWrite);
		}

		fclose(inputFile);
		fclose(outputFile);
	}

	return 0;
}