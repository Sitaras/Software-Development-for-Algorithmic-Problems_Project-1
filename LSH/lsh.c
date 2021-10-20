#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "../Vector/vector.h"
#include "../hashTable/hashTable.h"
#include "./helperFunctions.h"


extern int w;
extern int d;
extern int k;
extern int hashTableSize;


typedef struct hfunc{
  double *v;
  double t;
}h_function;

typedef struct gfunc{
  h_function *h_functions;
  int *r;
  int m;
}g_function;

typedef struct lsh_n{
  g_function *g_fun;
  HashTable *hts;
  int l;
}lshNode;
typedef lshNode *LSH;

/* H FUNCTIONS*/

void generateH(h_function *hfun){
  hfun->v=malloc(d*sizeof(double));
  for(int i=0;i<d;i++){
    hfun->v[i] = normalRandom();
  }
  hfun->t=uniform_distribution(0,w);
}

void destroyH(h_function h){
  free(h.v);
}

int computeH(h_function hfun,Vector vector){
  double pv = dot_product(hfun.v,getCoords(vector),d);
  double temp = (double) (pv+hfun.t)/(double)w;
  return floor(temp);
}

/* G FUNCTIONS*/

void generateG(g_function *gfun){
  gfun->h_functions = malloc(k*sizeof(h_function));
  for(int i=0;i<k;i++){
     generateH(&gfun->h_functions[i]);
  }
  gfun->r = malloc(k*sizeof(int));
  for(int i=0;i<k;i++){
     gfun->r[i]=rand();

  }
  gfun->m=(UINT_MAX-4);
}

void destroyG(g_function g){
  for(int i=0;i<k;i++){
    destroyH(g.h_functions[i]);
  }
  free(g.h_functions);
  free(g.r);
}

int computeG(g_function gfun,Vector p){
  int sum = 0;
  for(int i=0;i<k;i++){
    sum += mod(gfun.r[i]*computeH(gfun.h_functions[i],p),gfun.m);
  }
  return mod(mod(sum,gfun.m),hashTableSize);
}

/* LSH IMPLEMENTATION*/

LSH initializeLSH(int l){
  LSH tempLSH = malloc(sizeof(lshNode));
  tempLSH->g_fun = malloc(l*sizeof(g_function));
  tempLSH->hts = malloc(l*sizeof(HashTable));
  for(int i=0;i<l;i++){
     generateG(&(tempLSH->g_fun[i]));
     tempLSH->hts[i] = htInitialize(hashTableSize);
  }
  tempLSH->l=l;
  return tempLSH;
}

void destroyLSH(LSH lsh){
  for(int i=0;i<lsh->l;i++){
     destroyG(lsh->g_fun[i]);
     htDelete(lsh->hts[i]);
  }
  free(lsh);
}
