#ifndef HASH_TABLE_H
#define HASH_TABLE_H


typedef struct hashtable_head *HashTable;

int hashFunction(const HashTable,int);
HashTable htInitialize(int);
int htInsert(HashTable ,Vector, int);
// hashtable_nodePtr htSearch(const HashTable,int);
void htPrint(const HashTable);
HashTable htDelete(HashTable,int );
int htDeleteNode(HashTable,int);

#endif
