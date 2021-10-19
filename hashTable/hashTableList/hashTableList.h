#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct listNode *List;

List allocateListNode(vector);
List listInsert(List, vector);
List listSearchId(List ,int );
void listPrint(List );
List listDelete(List );

#endif
