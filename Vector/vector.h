#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec_node *Vector;

Vector initVector(double *, char []);

Vector copyVector(Vector );

void deleteVector(Vector);

void printVector(Vector );
void printVectorId(Vector );

void printVectorInFile(Vector,FILE* );
void printVectorIdInFile(Vector,FILE* );

int compareVectors(Vector , Vector );

// Vector initVectorCluster(double *);
// void initVectorConflictArr(Vector ,int );
// void setVectorConflictArrZero(Vector ,int );
// void setVectorConflictArrIndex(Vector ,int );
// void vectorCheckFlag(Vector ,int );
// void vectorAssignFlag(Vector ,int );
// int getVectorCheckFlag(Vector );
// int getVectorAssignFlag(Vector );

int assignedToCluster(Vector );

int getAssignedCluster(Vector );

void setAssignedCluster(Vector ,int );

double *getCoords(Vector);

void initializeClusterInfo(Vector );
int getAssignedIteration(Vector );
void setAssignedIteration(Vector ,int );
double getAssignedAtRadius(Vector );
void setAssignedAtRadius(Vector ,double );

#endif
