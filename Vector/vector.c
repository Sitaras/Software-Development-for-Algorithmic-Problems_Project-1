#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
extern int d;


typedef struct vec_node{
  // add the ID of vector
  double* coords;
  int assignedCluster;
}vec;
typedef vec *Vector;

double* getCoords(Vector v){
  return v->coords;
}

int assignedToCluster(Vector v){
  return (v->assignedCluster==-1) ? FALSE : TRUE;
}

int getAssignedCluster(Vector v){
  return v->assignedCluster;
}

void setAssignedCluster(Vector v,int cluster){
   v->assignedCluster = cluster;
}

// void initVectorConflictArr(Vector v,int numOfClusters){
//   v->clusterInfo->conflictArr=malloc(numOfClusters*sizeof(int));
//   for(int i=0;i<numOfClusters;i++){
//     v->clusterInfo->conflictArr[i]=0;
//   }
// }
//
// void setVectorConflictArrZero(Vector v,int numOfClusters){
//   for(int i=0;i<numOfClusters;i++){
//     v->clusterInfo->conflictArr[i]=0;
//   }
// }
// void setVectorConflictArrIndex(Vector v,int index){
//     v->clusterInfo->conflictArr[index]=1;
// }
//
// void vectorCheckFlag(Vector v,int i){
//   v->clusterInfo->checkFlag=i;
// }
//
// void vectorAssignFlag(Vector v,int i){
//   v->clusterInfo->assignFlag=i;
// }
//
// int getVectorCheckFlag(Vector v){
//   return v->clusterInfo->checkFlag;
// }
//
// int getVectorAssignFlag(Vector v){
//   return v->clusterInfo->assignFlag;
// }


Vector initVector(double *vec){
  Vector v=malloc(sizeof(struct vec_node));
  v->coords = malloc(d*sizeof(double));
  for(int i=0;i<d;i++){
    (v->coords)[i] = vec[i];
  }
  v->assignedCluster = -1;
  return v;
}


Vector copyVector(Vector vec){
  double *coords = getCoords(vec);
  Vector v=malloc(sizeof(struct vec_node));
  v->coords = malloc(d*sizeof(double));
  for(int i=0;i<d;i++){
    (v->coords)[i] = coords[i];
  }
  return v;
}


void deleteVector(Vector v){
  free(v->coords);
  free(v);
}


void printVector(Vector v){
  if(v==NULL)
    return;
  printf("\n[");
  for(int i=0;i<d;i++){
    printf(" %f",v->coords[i]);
  }
  printf(" ]\n");
}

void printVectorInFile(Vector v,FILE *fptr){
  if(v==NULL)
    return;
  fprintf(fptr,"\n[");
  for(int i=0;i<d;i++){
    fprintf(fptr," %f",v->coords[i]);
  }
  fprintf(fptr," ]\n");
}

int compareVectors(Vector v1,Vector v2){
  for(int i=0;i<d;i++){
    if(v1->coords[i]!=v2->coords[i])
      return 0;
  }
  return 1;
}
