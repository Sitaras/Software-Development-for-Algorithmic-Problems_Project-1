#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include "./hashTableList/hashTableList.h"

typedef struct hashtable_node hashtable_node;
typedef struct hashtable hashtable;

// hash table with separate chaining
// each bucket of hash table point to a linked list (bucket)

struct hashtable_node{
  int key;
  listNode* head;
};

struct hashtable{
  hashtable_node* table;
  int buckets;
  int numberOfNodes;
};

int hashFunction(const hashtable*,int);
void htInitialize(hashtable *,int);
int htUniqueInsert(hashtable* ,vector);
node* htSearch(const hashtable*,int);
void htPrint(const hashtable*);
void htDelete(hashtable*);
int htDeleteNode(hashtable*,int);

#endif
