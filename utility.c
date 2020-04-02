#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

int fileOpenError(char* fileName)
{
    printf("Error opening the file: %s\n", fileName);
    exit(0);
}

int error(char* str)
{
    printf("%s\n", str);
}

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
