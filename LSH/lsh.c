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

int getL(LSH lsh){
  return lsh->l;
}
HashTable *getHts(LSH lsh){
  return lsh->hts;
}
g_function *getGfuns(LSH lsh){
  return lsh->g_fun;
}

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

int computeG(g_function gfun,Vector p,int *id){
  int sum = 0;
  for(int i=0;i<k;i++){
    sum += mod(gfun.r[i]*computeH(gfun.h_functions[i],p),gfun.m);
  }
  int temp_ID = mod(sum,gfun.m);
  (*id) = temp_ID;
  return mod(temp_ID,hashTableSize);
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

void insertToLSH(LSH lsh,Vector v){
  int l = lsh->l;
  for(int i=0;i<l;i++){
    int id;
    int index = computeG(lsh->g_fun[i],v,&id);
    htInsert(lsh->hts[i],v,index,id);
  }
}

void printLSH(LSH lsh){
  int l = lsh->l;
  for(int i=0;i<l;i++){
    printf("-------- HASH %d --------\n",i+1);
    htPrint(lsh->hts[i]);
    printf("--------------------------\n");
  }
}

void destroyLSH(LSH lsh){
  for(int i=0;i<lsh->l;i++){
     destroyG(lsh->g_fun[i]);
     htDelete(lsh->hts[i],!i);
  }
  free(lsh);
}


void nearestNeigbor(LSH lsh,Vector q){
  printf("ABOUT TO SEARCH NEAREST NEIGHBOR FOR : ");
  printVector(q);
  Vector nearest=NULL;
  double nearestDist=-1;
  int l = getL(lsh);
  HashTable *hts = getHts(lsh);
  g_function *gfuns = getGfuns(lsh);
  for(int i=0;i<l;i++){
    int q_ID;
    int q_index = computeG(gfuns[i],q,&q_ID);
    htFindNearestNeighbor(hts[i],q_index,q,&nearest,&nearestDist,d,q_ID);
  }
  if(nearestDist>=0 && nearest!=NULL){
    printf("FOUND NEAREST NEIGHBOR ");
    printVector(nearest);
    printf("WITH DISTANCE =  %f\n",nearestDist);
  }else{
    printf("- DID NOT FIND NEAREST NEIGHBOR\n");
  }
}

void kNearestNeigbors(LSH lsh,Vector q,int k){
  printf("ABOUT TO SEARCH %d NEAREST NEIGHBORS FOR : ",k);
  printVector(q);
  Vector nearest[k];
  double knearestDists[k];
  for (int i = 0; i < k; i++){
    knearestDists[i]=-1;
    nearest[i]=NULL;
  }

  int l = getL(lsh);
  HashTable *hts = getHts(lsh);
  g_function *gfuns = getGfuns(lsh);
  for(int i=0;i<l;i++){
    int q_ID;
    int q_index = computeG(gfuns[i],q,&q_ID);
    htKFindNearestNeighbors(hts[i], q_index, q, nearest, knearestDists, d,k,q_ID);
  }
  int flag=1;
  for (int i = k-1; i >= 0; i--){
    if (knearestDists[i] >= 0 && nearest[i] != NULL){
      printf("FOUND %d NEAREST NEIGHBOR: ",i);
      printVector(nearest[i]);
      printf("WITH DISTANCE =  %f\n", knearestDists[i]);
      flag=0;
    }
  }
  if(flag){
    printf("- DID NOT FIND NEAREST NEIGHBOR\n");
  }
}
