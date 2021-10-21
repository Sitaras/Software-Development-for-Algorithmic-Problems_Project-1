#ifndef LIST_H
#define LIST_H



typedef struct listNode *List;
typedef struct hashtable_head *HashTable;

List initializeList();
List allocateListNode(Vector ,int );
List listInsert(List, Vector,int );
List listUniqueInsert(List ,Vector ,int );
List listSearchId(List ,int );
void listPrint(List );
void listRangePrint(List ,Vector ,int );
List listDelete(List ,int );

void listFindNearestNeighbor(List ,Vector ,Vector *,double *,int ,int );
void listFindKNearestNeighbors(List, Vector, Vector *, double *, int,int ,int );

void listFindNeighborsInRadius(List ,HashTable ,Vector ,int ,int ,int );

void listFindNearestNeighborCube(List ,Vector ,Vector *,double *,int ,int *,int );
void listFindKNearestNeighborsCube(List ,Vector ,Vector *,double *,int ,int ,int *,int );
#endif
