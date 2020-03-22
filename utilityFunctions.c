#include <stdio.h>
#include <stdlib.h>

void fileNotNull(FILE *file, char *fileName)
{
	if (file == NULL)
	{
		fprintf(stderr, "ERROR: File %s unable to open.\n", fileName);
		exit(0);
	}
}