HASHTABLELIST=./hashTable/hashTableList
HASHTABLE=./hashTable
PARSING=./parsing
VECTOR=./Vector
LSH=./LSH

CC=gcc
CFLAGS= -g -Wall -I$(HASHTABLELIST) -I$(HASHTABLE) -I$(PARSING) -I$(VECTOR) -I$(LSH)

OBJ1= main.o $(HASHTABLE)/hashTable.o $(HASHTABLELIST)/hashTableList.o $(PARSING)/parsing.o $(VECTOR)/vector.o  $(LSH)/lsh.o $(LSH)/helperFunctions.o
OBJ2= mainCube.o

EXEC1 = demo
EXEC2 = demoCube

all: $(EXEC1) $(EXEC2)

$(EXEC1): $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o $(EXEC1) -lm

$(EXEC2): $(OBJ2)
	$(CC) $(CFLAGS) $(OBJ2) -o $(EXEC2) -lm


.PHONY: clean

clean:
	rm -f $(OBJ1) $(OBJ2) $(EXEC1) $(EXEC2)
