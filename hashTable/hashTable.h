#ifndef HASH_TABLE_H
#define HASH_TABLE_H


typedef struct hashtable_head *HashTable;
typedef struct listNode *List;

int hashFunction(const HashTable,int);
HashTable htInitialize(int);
int htInsert(HashTable ,Vector, int,int );
// hashtable_nodePtr htSearch(const HashTable,int);
void htPrint(const HashTable);
void htRangePrint(const HashTable ,Vector ,int,FILE* );
void htRangeDelete(HashTable , Vector ,int ,int );
HashTable htDelete(HashTable,int );
int htDeleteNode(HashTable,int);

void htFindNearestNeighbor(HashTable ,int ,Vector ,Vector *,double *,int ,int );
void htKFindNearestNeighbors(HashTable, int, Vector, Vector *, double *, int ,int ,int );
void htFindNeighborsInRadiusClustering(HashTable ,int ,int ,List* ,HashTable ,Vector ,int ,int ,int ,int *);
void htFindNeighborsInRadius(HashTable ,int ,HashTable ,Vector ,int ,int ,int );
void htRangeInsert(HashTable , Vector ,int ,int );

void htFindNearestNeighborCube(HashTable ,int ,Vector ,Vector *,double *,int ,int *,int );
void htKFindNearestNeighborsCube(HashTable ,int ,Vector ,Vector *,double *,int ,int ,int *,int );
void htFindNeighborsInRadiusCube(HashTable ,int ,HashTable ,Vector ,int ,int ,int *,int );

Vector htMeanOfCluster(HashTable ,int );
#endif
