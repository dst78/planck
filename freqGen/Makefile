CC=gcc

default: 
	echo compiling new binary
	$(CC) -o genVOctTable main.c
	
clean:
	echo removing binary
	rm genVOctTable

write:
	./genVOctTable > ../vOctTable.h
	
all: clean default write
