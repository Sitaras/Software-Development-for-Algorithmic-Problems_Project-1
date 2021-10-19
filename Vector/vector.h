#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec_node *Vector;

Vector initVector(double *);

void deleteVector(Vector);

void printVector(Vector );

double *getCoords(Vector );

#endif
