#ifndef LIST_H
#define LIST_H



typedef struct listNode *List;

List initializeList();
List allocateListNode(Vector);
List listInsert(List, Vector);
List listSearchId(List ,int );
void listPrint(List );
List listDelete(List ,int );

void listFindNearestNeighbor(List ,Vector ,Vector *,double *,int );

#endif
