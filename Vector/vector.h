#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec_node *Vector;

Vector initVector(double *);

Vector copyVector(Vector );

void deleteVector(Vector);

void printVector(Vector );

void printVectorInFile(Vector,FILE* );

int compareVectors(Vector , Vector );

double *getCoords(Vector);

#endif
