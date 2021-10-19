HASHTABLELIST=./hashTable/hashTableList
HASHTABLE=./hashTable
PARSING=./hashTable

CC=gcc
CFLAGS= -g -Wall -I$(HASHTABLELIST) -I$(HASHTABLE) -I$(PARSING)

OBJ1= main.o $(HASHTABLE)/hashTable.o $(HASHTABLELIST)/hashTableList.o $(PARSING)/parsing.o

EXEC= demo

all: $(EXEC)

$(EXEC): $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o $(EXEC)


.PHONY: clean

clean:
	rm -f $(OBJ1) $(EXEC)
