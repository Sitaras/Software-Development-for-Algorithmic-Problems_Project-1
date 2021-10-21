#ifndef HYPERCUBE_H
#define HYPERCUBE_H

typedef struct hc_n *HyperCube;

HyperCube initializeHyperCube();

void insertToHyperCube(HyperCube ,Vector );

void printHyperCube(HyperCube );

void deleteHyperCube(HyperCube );

#endif
