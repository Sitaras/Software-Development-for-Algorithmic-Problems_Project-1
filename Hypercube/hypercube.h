#ifndef HYPERCUBE_H
#define HYPERCUBE_H

typedef struct hc_n *HyperCube;

HyperCube initializeHyperCube();

void insertToHyperCube(HyperCube ,Vector );

void printHyperCube(HyperCube );

void deleteHyperCube(HyperCube );

void nearestNeigborHypercube(HyperCube ,Vector ,int ,int,FILE* );

void kNearestNeigborsHypercube(HyperCube ,Vector ,int ,int ,int,double *,FILE* );

void radiusNeigborHypercube(HyperCube ,Vector ,double ,int ,int,FILE* );
void radiusNeigborHypercubeClustering(HyperCube ,Vector ,HashTable ,double ,int ,int ,int ,List* ,int *,int );

#endif
