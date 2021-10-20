HASHTABLELIST=./hashTable/hashTableList
HASHTABLE=./hashTable
PARSING=./parsing
VECTOR=./Vector
LSH=./LSH

CC=gcc
CFLAGS= -g -Wall -I$(HASHTABLELIST) -I$(HASHTABLE) -I$(PARSING) -I$(VECTOR) -I$(LSH)

OBJ1= main.o $(HASHTABLE)/hashTable.o $(HASHTABLELIST)/hashTableList.o $(PARSING)/parsing.o $(VECTOR)/vector.o  $(LSH)/lsh.o $(LSH)/helperFunctions.o

EXEC= demo

all: $(EXEC)

$(EXEC): $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o $(EXEC)


.PHONY: clean

clean:
	rm -f $(OBJ1) $(EXEC)
