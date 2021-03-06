# Variable declaration.
CC = gcc
flags = -c
executableName = test
driver = mainDriver
outputFile = outputFile.txt
statisticsFile = outputFile_processStatistics.txt

# Makes
all: $(driver).o kernel.o tlb.o cache.o pcb.o pageTable.o segmentTable.o frameTable.o utility.o createSegmentationInput.o
	$(CC) $(driver).o kernel.o tlb.o cache.o pcb.o pageTable.o segmentTable.o frameTable.o utility.o createSegmentationInput.o -o $(executableName)


$(driver).o: $(driver).c
	$(CC) $(flags) $(driver).c

kernel.o: kernel.c
	$(CC) $(flags) kernel.c

tlb.o: tlb.c
	$(CC) $(flags) tlb.c

cache.o: cache.c
	$(CC) $(flags) cache.c
	
pcb.o: pcb.c
	$(CC) $(flags) pcb.c
	
pageTable.o: pageTable.c
	$(CC) $(flags) pageTable.c

segmentTable.o: segmentTable.c
	$(CC) $(flags) segmentTable.c

frameTable.o: frameTable.c
	$(CC) $(flags) frameTable.c

utility.o: utility.c
	$(CC) $(flags) utility.c

createSegmentationInput.o: createSegmentationInput.c
	$(CC) $(flags) createSegmentationInput.c

# To clean the compilation.
clean:
	rm -f *.o $(outputFile) $(statisticsFile) $(executableName) Segment_*.txt