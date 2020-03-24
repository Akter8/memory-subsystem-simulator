#include <stdio.h>
#include <stdlib.h>


/*
 * Finds an error in the opened file if any.
 */
void fileNotNull(FILE *file, char *fileName)
{
	if (file == NULL)
	{
		fprintf(stderr, "ERROR: File %s unable to open.\n", fileName);
		exit(0);
	}
}