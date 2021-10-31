#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "../Vector/vector.h"
#include "../hashTable/hashTable.h"
#include "../hashTable/hashTableList/hashTableList.h"
#include "./helperFunctions.h"


extern int w;
extern int d;
extern int k_LSH;
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

void generateH_LSH(h_function *hfun){
  // generate v vector coordinates of h function, v ∼ N (0, 1)^d
  hfun->v=malloc(d*sizeof(double));
  for(int i=0;i<d;i++){
    hfun->v[i] = normalRandom();
  }
  // pick t variable uniformly ∈ R [0, w).
  hfun->t=uniform_distribution(0,w);
}

void destroyH_LSH(h_function h){
  free(h.v);
}

int computeH_LSH(h_function hfun,Vector vector){
  // compute the dot product of the given vector with the v vector of h function (p · v)
  double pv = dot_product(hfun.v,getCoords(vector),d);
  // finally calculate the value of h function
  double temp = (double) (pv+hfun.t)/(double)w;
  return floor(temp);
}

/* G FUNCTIONS*/

void generateG(g_function *gfun){
  // allocate and generate the h functions tha will be used at the calculation of G function, k_LSH (number of h functions) has been given from the command line
  // g is a random combination of hi's, every g function has k_LSH h functions
  gfun->h_functions = malloc(k_LSH*sizeof(h_function));
  for(int i=0;i<k_LSH;i++){
     generateH_LSH(&gfun->h_functions[i]);
  }
  // allocate and generate as many variables r (r[i] are a random int ≤ 32 bits) as the functions h
  gfun->r = malloc(k_LSH*sizeof(int));
  for(int i=0;i<k_LSH;i++){
     gfun->r[i]=rand();
  }
  gfun->m=(UINT_MAX-4);
}

void destroyG(g_function g){
  for(int i=0;i<k_LSH;i++){
    destroyH_LSH(g.h_functions[i]);
  }
  free(g.h_functions);
  free(g.r);
}

int computeG(g_function gfun,Vector p,int *id){
  int sum = 0;
  // g(p) = [(r1h1(p) + r2h2(p) + · · · + rkhk (p)) mod M] mod TableSize
  // g is a random combination of hi's, every g function has k_LSH h functions
  // compute g function and at the same time compute all the h functions that make it up
  // with this property of modulus operator overflow is avoided ((a _ b) mod m = ((a mod m)(b mod m)) mod m)
  for(int i=0;i<k_LSH;i++){
    sum += mod(gfun.r[i]*computeH_LSH(gfun.h_functions[i],p),gfun.m);
  }
  int temp_ID = mod(sum,gfun.m);
  // Store object ID along with pointer to object (Querying trick), for all bucket elements to avoid to compute g Euclidean distance for all vectors p in bucket
  // do it only for p which: ID(p) = ID(q)
  (*id) = temp_ID;
  return mod(temp_ID,hashTableSize);
}


/* LSH IMPLEMENTATION*/

LSH initializeLSH(int l){
  LSH tempLSH = malloc(sizeof(lshNode));
  // allocate as many G functions as the number of hash tables (g functions used like hash functions at the corresponding  hash tables )
  tempLSH->g_fun = malloc(l*sizeof(g_function));
  // allocate the hash tables that the LSH need
  tempLSH->hts = malloc(l*sizeof(HashTable));
  printf("*!*!**!*!* HASHSIZE = %d\n",hashTableSize);
  // generate G functions and initialize the correspodings hash tables
  for(int i=0;i<l;i++){
     generateG(&(tempLSH->g_fun[i]));
     tempLSH->hts[i] = htInitialize(hashTableSize);
  }
  // save l (the number of hash tables)
  tempLSH->l=l;
  // and finally return the LSH node
  return tempLSH;
}

void insertToLSH(LSH lsh,Vector v){
  // insert the given vector in all LSΗ hash tables
  // the bucket of the hash table that the vector will be inserted depends from the corresponding g function of the specific hash Table (hash function)
  // at the new node tha will be inserted at the hash Tables save the id (Querying trick)
  int l = lsh->l;
  for(int i=0;i<l;i++){
    int id;
    int index = computeG(lsh->g_fun[i],v,&id);
    htInsert(lsh->hts[i],v,index,id);
  }
}

void insertFromListToLSH(List list,LSH lsh){
  // insert every vector of the list at the corresponding LSH
  if(list==NULL){ return;}
  List temp=list;
  while(temp!=NULL){
      insertToLSH(lsh,getVector(temp));
      temp=getNext(temp);
  }
}

void printLSH(LSH lsh){
  // just print the LSH
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
  free(lsh->g_fun);
  free(lsh->hts);
  free(lsh);
}


void nearestNeigborLSH(LSH lsh,Vector q,FILE *fptr){
  // find the nearest neighbor of the given vector q
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
    fprintf(fptr,"FOUND NEAREST NEIGHBOR ");
    printVectorId(nearest);
    printVectorIdInFile(nearest,fptr);
    fprintf(fptr,"distanceLSH: %f\n",nearestDist);
  }else{
    fprintf(fptr,"- DID NOT FIND NEAREST NEIGHBOR\n");
  }
}

void kNearestNeigborsLSH(LSH lsh,Vector q,int knn,double *knearestTrueDists,FILE* fptr){
  Vector nearest[knn];
  double knearestDists[knn];
  for (int i = 0; i < knn; i++){
    knearestDists[i]=-1;
    nearest[i]=NULL;
  }

  int l = getL(lsh);
  HashTable *hts = getHts(lsh);
  g_function *gfuns = getGfuns(lsh);
  for(int i=0;i<l;i++){
    int q_ID;
    int q_index = computeG(gfuns[i],q,&q_ID);
    htKFindNearestNeighbors(hts[i], q_index, q, nearest, knearestDists, d,knn,q_ID);
  }
  int flag=1;
  for (int i = knn-1; i >= 0; i--){
    if (knearestDists[i] >= 0 && nearest[i] != NULL){
      fprintf(fptr,"Nearest neighbor-%d: ",knn-i);
      printVectorIdInFile(nearest[i],fptr);
      fprintf(fptr,"distanceLSH: %f\n", knearestDists[i]);
      fprintf(fptr,"distanceTrue: %f\n", knearestTrueDists[i]);
      flag=0;
    }
  }
  if(flag){
    fprintf(fptr,"- DID NOT FIND NEAREST NEIGHBOR\n");
  }
}

void radiusNeigborLSH(LSH lsh,Vector q,double radius,FILE *fptr){

  int vecsInRadius_size=1;
  int l = getL(lsh);
  HashTable *hts = getHts(lsh);
  g_function *gfuns = getGfuns(lsh);
  if(l>0){
    vecsInRadius_size = getNumberOfVectors(hts[0])/8;
  }
  HashTable vecsInRadius = htInitialize(vecsInRadius_size); // TODO: CHANGE SIZE
  for(int i=0;i<l;i++){
    int q_ID;
    int q_index = computeG(gfuns[i],q,&q_ID);
    htFindNeighborsInRadius(hts[i],q_index,vecsInRadius,q,d,q_ID,radius);
  }
  htRangePrint(vecsInRadius,q,d,fptr);

  htDelete(vecsInRadius,0);
}
void radiusNeigborClustering(LSH lsh,Vector q,double radius,HashTable vecsInRadius,int centroidIndex,List* confList,int *assignCounter,int iteration){
  int l = getL(lsh);
  HashTable *hts = getHts(lsh);
  g_function *gfuns = getGfuns(lsh);
  for(int i=0;i<l;i++){
    int q_ID;
    int q_index = computeG(gfuns[i],q,&q_ID);
    htFindNeighborsInRadiusClustering(hts[i],q_index,centroidIndex,confList,vecsInRadius,q,d,q_ID,radius,assignCounter,iteration);
  }
}
