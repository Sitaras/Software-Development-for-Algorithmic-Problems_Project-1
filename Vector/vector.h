#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec_node *Vector;

Vector initVector(double *);

Vector copyVector(Vector );

void deleteVector(Vector);

void printVector(Vector );

void printVectorInFile(Vector,FILE* );

int compareVectors(Vector , Vector );

void initVectorConflictArr(Vector ,int );
void setVectorConflictArrZero(Vector ,int );
void setVectorConflictArrIndex(Vector ,int );
void vectorCheckFlag(Vector ,int );
void vectorAssignFlag(Vector ,int );
int getVectorCheckFlag(Vector );
int getVectorAssignFlag(Vector );

double *getCoords(Vector);

#endif
