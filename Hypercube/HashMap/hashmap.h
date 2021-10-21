#ifndef HASHMAP_H
#define HASHMAP_H

typedef int Key;
typedef int Value;

typedef struct node *Record;
typedef struct hash *HashMap;


Key getKey(Record );
Value getValue(Record );

HashMap htCreate(int );

Record htSearchOrInsert(HashMap ,Key ,Value );

Record htSearch(HashMap ,Key );

void htDestroy(HashMap );



#endif
