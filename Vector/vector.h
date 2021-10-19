#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec_node *Vector;

Vector initVector(int *);

void deleteVector(Vector);

void printVector(Vector );

#endif
