#include <stdio.h>
#include <stdlib.h>

extern int d;

typedef struct vec_node{
  int* coords;
}vec;
typedef vec *Vector;


Vector initVector(int *vec){
  Vector v=malloc(sizeof(struct vec_node));
  v->coords = malloc(d*sizeof(int));
  for(int i=0;i<d;i++){
    (v->coords)[i] = vec[i];
  }
  return v;
}

void deleteVector(Vector v){
  free(v->coords);
  free(v);
}


void printVector(Vector v){
  printf("\n[");
  for(int i=0;i<d;i++){
    printf(" %d",v->coords[i]);
  }
  printf(" ]\n");
}
