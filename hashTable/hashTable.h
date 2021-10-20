#ifndef HASH_TABLE_H
#define HASH_TABLE_H


typedef struct hashtable_head *HashTable;

int hashFunction(const HashTable,int);
HashTable htInitialize(int);
int htInsert(HashTable ,Vector, int,int );
// hashtable_nodePtr htSearch(const HashTable,int);
void htPrint(const HashTable);
void htRangePrint(const HashTable ,Vector ,int );
HashTable htDelete(HashTable,int );
int htDeleteNode(HashTable,int);

void htFindNearestNeighbor(HashTable ,int ,Vector ,Vector *,double *,int ,int );
void htKFindNearestNeighbors(HashTable, int, Vector, Vector *, double *, int ,int ,int );

void htFindNeighborsInRadius(HashTable ,int ,HashTable ,Vector ,int ,int ,int );
void htRangeInsert(HashTable , Vector ,int ,int );

#endif
