# Variable declaration.
CC = gcc
flags = -c
executableName = test
driver = driver2
outputFile = outputFile.txt

# Makes
all: $(driver).o tlb.o cache.o pcb.o pageTable.o segmentTable.o frameTable.o utilityFunctions.o createSegmentationInput.o
	$(CC) $(driver).o tlb.o cache.o pcb.o pageTable.o segmentTable.o frameTable.o utilityFunctions.o createSegmentationInput.o -o $(executableName)


$(driver).o: $(driver).c
	$(CC) $(flags) $(driver).c

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

utilityFunctions.o: utilityFunctions.c
	$(CC) $(flags) utilityFunctions.c

createSegmentationInput.o: createSegmentationInput.c
	$(CC) $(flags) createSegmentationInput.c

# To clean the compilation.
clean:
	rm -f *.o $(outputFile) $(executableName) Segment_*.txt