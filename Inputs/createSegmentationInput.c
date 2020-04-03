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

	char *inputFileName[5] = {"APSI.txt", "LI.txt", "CC1.txt", "M88KSIM.txt", "VORTEX.txt"};

	for (int i = 0; i < 5; ++i)
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
			readOrWrite = findReadOrWriteMemoryAccess();
			readWrite = 'r';

			if (readOrWrite == WRITE)
				readWrite = 'w';

			if (inputChar == '7')
				// Printing in hex.
	   			fprintf(outputFile, "8 %c\n", readWrite);
	   		else
	   			fprintf(outputFile, "0 %c\n", readWrite);
		}


		fclose(inputFile);
		fclose(outputFile);
	}

	return 0;
}