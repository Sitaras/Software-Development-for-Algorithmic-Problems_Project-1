#ifndef HYPERCUBE_H
#define HYPERCUBE_H

typedef struct hc_n *HyperCube;

HyperCube initializeHyperCube();

void insertToHyperCube(HyperCube ,Vector );

void printHyperCube(HyperCube );

void deleteHyperCube(HyperCube );

void nearestNeigbor(HyperCube ,Vector ,int ,int );

void kNearestNeigbors(HyperCube ,Vector ,int ,int ,int );

void radiusNeigbor(HyperCube ,Vector ,double ,int ,int );

#endif
