#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct listNode listNode;
typedef struct node node;
typedef struct vector vector;

// hash table's linked list

struct vector{
  int id;
  unsigned int bit;
};

struct node{
  int id;
  unsigned int bit;
};

struct listNode {
  node n;
  listNode* next;
};

listNode* allocateListNode(vector);
listNode* listInsert(listNode *, vector);
listNode* listSearchId(listNode * ,int );
void listPrint(listNode *);
listNode* listDelete(listNode *);

#endif
