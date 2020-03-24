# Variable declaration.
CC = gcc
flags = -c
executableName = test
driver = 
outputFile = outputFile.txt

all: $(driver).o 
	$(CC) $(driver).o -o $(executableName)

$(driver).o: $(driver).c
	$(CC) $(flags) $(driver).c


# To remove all .o files and to clean the directory.
clean:
	rm -f *.o $(executableName) $(outputFile)