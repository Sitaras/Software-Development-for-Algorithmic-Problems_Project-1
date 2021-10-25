#include <stdio.h>
#include <stdlib.h>

extern int d;

typedef struct vec_node{
  // add the ID of vector
  double* coords;
}vec;
typedef vec *Vector;

double *getCoords(Vector v){
  return v->coords;
}


Vector initVector(double *vec){
  Vector v=malloc(sizeof(struct vec_node));
  v->coords = malloc(d*sizeof(double));
  for(int i=0;i<d;i++){
    (v->coords)[i] = vec[i];
  }
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
