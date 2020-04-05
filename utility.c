#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE* outputFile;

void fileNotNull(FILE *file, char *fileName)
{
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: File %s unable to open.\n", fileName);
        exit(0);
    }
}

int fileOpenError(char* fileName)
{
    printf("Error opening the file: %s\n", fileName);
    exit(0);
}

int error(char* str)
{
    printf("%s\n", str);
}


/*
 * Reads the linear address data from the input file.
 */
int readAddr(FILE *fp)
{
    int addr;
    if(fscanf(fp,"%x",&addr)!=EOF)
        return addr;
    else{
       // printf("eof\n");
        return -1;
    }

}

/*
 * Reads the segment number data from the other input file (That will be created in the driver function).
 
int4 readSegNum(FILE *fp, char *write)
{
    int4 val;
   int x;
    if(fscanf(fp,"%x %c",&x,write)!=EOF){
        val.value = x;
        return val;
    }
    else{
       // printf("eof\n");
        val.value=-1;
        return val;
    }
}
*/
// Obtains FileNameInputs from input.txt and also creates separate files for segmentNumbers, corresponding to each reference
void ObtainFileNameInput(FILE* input, int n, char SegAddrInputFileName[][100], char LinearAddrInputFileName[][100])
{
    for(int i = 0; i < n; ++i)
    {
        fscanf(input, "%s", LinearAddrInputFileName[i]); 
    }
    fprintf(outputFile, "Driver: Linear adrress input file names received\n");

    // Create the segment number input files for the corresponding linear address files.
    createSegmentFiles(LinearAddrInputFileName, n);

    // Segment number input files will have the same names, but with a prefix of "Segment_".
    for (int i = 0; i < n; ++i)
    {
        strcpy(SegAddrInputFileName[i], "Segment_");
        strcat(SegAddrInputFileName[i], LinearAddrInputFileName[i]);
    }
    fprintf(outputFile, "Driver: Segment number input file names received\n");

}


/*
unsigned int readAddr(FILE* inputFile)
{
    //Stores the line read from inputFile
    char temp[9];
    fscanf(inputFile, "%s", temp);
    
    unsigned int Addr = 0;
    unsigned int mult = 1;
    //Converts read line from file, Hexadecimal into decimal
    for(int i = 7; i >= 0; --i)
    {
        if(48 <= temp[i] && temp[i] <= 57)
        {
            Addr = Addr+(mult*(unsigned int)(temp[i]-48));
        }
        else
        {
            Addr = Addr+(mult*(unsigned int)(temp[i]-87));
        }
        mult = mult*16;
    }
    return Addr;
}

int readSegNum(FILE* SegNumInputFile, char* ReadWrite)
{
   char temp[5];
   fscanf(SegNumInputFile, "%s", temp);

    unsigned int Addr = 0;
    unsigned int mult = 1;
    //Converts read line from file, Hexadecimal into decimal
    for(int i = 4; i >= 0; --i)
    {
        if(48 <= temp[i] && temp[i] <= 57)
        {
            Addr = Addr+(mult*(unsigned int)(temp[i]-48));
        }
        else
        {
            Addr = Addr+(mult*(unsigned int)(temp[i]-87));
        }
        mult = mult*2;
    }

    fscanf(SegNumInputFile, "%c", ReadWrite);

    return Addr;
}
*/
