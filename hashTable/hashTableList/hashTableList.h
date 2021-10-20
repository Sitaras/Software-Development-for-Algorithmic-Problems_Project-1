#ifndef LIST_H
#define LIST_H



typedef struct listNode *List;

List initializeList();
List allocateListNode(Vector ,int );
List listInsert(List, Vector,int );
List listSearchId(List ,int );
void listPrint(List );
List listDelete(List ,int );

void listFindNearestNeighbor(List ,Vector ,Vector *,double *,int ,int );
void listFindKNearestNeighbors(List, Vector, Vector *, double *, int,int ,int );
#endif
