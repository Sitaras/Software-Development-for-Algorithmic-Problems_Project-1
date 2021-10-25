#ifndef LSH_H
#define LSH_H

typedef struct lsh_n * LSH;

typedef struct listNode *List;
LSH initializeLSH(int );
void insertToLSH(LSH ,Vector );
void printLSH(LSH );
void destroyLSH(LSH );

void nearestNeigbor(LSH ,Vector,FILE* );
void kNearestNeigbors(LSH, Vector,int,FILE*);
void radiusNeigbor(LSH ,Vector ,double,FILE* );
void radiusNeigborClustering(LSH ,Vector ,double ,HashTable ,int ,List );
#endif
