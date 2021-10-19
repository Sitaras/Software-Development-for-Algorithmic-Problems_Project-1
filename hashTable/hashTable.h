#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include "./hashTableList/hashTableList.h"



int hashFunction(const hashtable*,int);
void htInitialize(hashtable *,int);
int htUniqueInsert(hashtable* ,vector);
node* htSearch(const hashtable*,int);
void htPrint(const hashtable*);
void htDelete(hashtable*);
int htDeleteNode(hashtable*,int);

#endif
