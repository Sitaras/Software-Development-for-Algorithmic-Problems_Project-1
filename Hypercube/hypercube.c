#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "../Vector/vector.h"
#include "../hashTable/hashTable.h"
#include "HashMap/hashmap.h"
#include "../LSH/helperFunctions.h"

extern int d;
extern int k;
extern int m;
extern int probes;
extern int w;

typedef struct hfunc{
  double *v;
  double t;
}h_function;

typedef struct hc_n{
  h_function *h_functions;
  HashMap *f_funs;
  HashTable hypercube;
}cubeNode;
typedef cubeNode *HyperCube;

/* Helper Functions */

int binaryToDecimal(int n)
{
    int num = n;
    int dec_value = 0;

    // Initializing base value to 1, i.e 2^0
    int base = 1;

    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;

        dec_value += last_digit * base;

        base = base * 2;
    }

    return dec_value;
}

int binaryArrayToDecimal(int s[],int size)
{
    int n = 0;
    int i;

    for (i = 0; i < size; ++i) {
        n <<= 1;
        n += s[i];
    }

    return n;
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

/* F Functions */

int flipCoin(){
  return rand()%2;
}

int computeF(HashMap f_fun,Key key){
  int bit = flipCoin();
  Record r = hmSearchOrInsert(f_fun,key,bit);
  return getValue(r);
}


HyperCube initializeHyperCube(){
  HyperCube hc = malloc(sizeof(cubeNode));
  hc->h_functions = malloc(k*sizeof(h_function));
  for(int i=0;i<k;i++){
     generateH(&(hc->h_functions[i]));
  }
  hc->f_funs = malloc(k*sizeof(HashMap));
  for(int i=0;i<k;i++){
    hc->f_funs[i] = hmCreate(100);  // starting size is 100, hashMap is resizable
  }
  hc->hypercube = htInitialize((int)pow(2,k));  // 2^k buckets
  return hc;
}

void insertToHyperCube(HyperCube hc,Vector v){
  int index=0;
  for(int i=0;i<k;i++){
    int h_result = computeH(hc->h_functions[i],v);
    int f_result = computeF(hc->f_funs[i],h_result);
    index *=10;
    index += f_result;
  }
  printf("INDEX = %d\n",index);
  int decimal_index = binaryToDecimal(index); // "hashTable key"
  htInsert(hc->hypercube,v,decimal_index,-1);
}

void printHyperCube(HyperCube hc){
    printf("-------- HyperCube --------\n");
    htPrint(hc->hypercube);
    printf("--------------------------\n");
}

void deleteHyperCube(HyperCube hc){
  for(int i=0;i<k;i++){
    destroyH(hc->h_functions[i]);
    hmDestroy(hc->f_funs[i]);
  }
  free(hc->f_funs);
  free(hc->h_functions);
  htDelete(hc->hypercube,1);
  free(hc);
}


void searchForHammingDistance(HyperCube hc,Vector v,int *v_index,int hammingDist,int startFrom,Vector *nearest,double *nearestDist,int *numOfSearched,int maxToSearch){
  if(hammingDist<=0){
    int new_index = binaryArrayToDecimal(v_index,k);
    printf("** HAMMING INDEX =%d\n",new_index);
    htFindNearestNeighborCube(hc->hypercube,new_index,v,nearest,nearestDist,d,numOfSearched,maxToSearch);
    return;
  }
  for(int i=startFrom;i<k;i++){
      v_index[i] = v_index[i]^1;
      searchForHammingDistance(hc,v,v_index,hammingDist-1,i+1,nearest,nearestDist,numOfSearched,maxToSearch);
      // search
      v_index[i] = v_index[i]^1;
      if((*numOfSearched)>=maxToSearch){
        break;
      }
  }
}

void nearestNeigbor(HyperCube hc,Vector q,int hammingDist,int m){
  printf("ABOUT TO SEARCH NEAREST NEIGHBOR FOR : ");
  printVector(q);
  Vector nearest=NULL;
  double nearestDist=-1;
  int index[k];
  int searched = 0;
  for(int i=0;i<k;i++){
    int h_result = computeH(hc->h_functions[i],q);
    int f_result = computeF(hc->f_funs[i],h_result);
    index[i] = f_result;
  }
  int index_decimal = binaryArrayToDecimal(index,k);
  printf("** INITIAL INDEX =%d\n",index_decimal);
  htFindNearestNeighborCube(hc->hypercube,index_decimal,q,&nearest,&nearestDist,d,&searched,m);

  for(int i=1;i<=hammingDist;i++){
    if(searched>=m){
      break;
    }
    searchForHammingDistance(hc,q,index,i,0,&nearest,&nearestDist,&searched,m);
  }

  if(nearestDist>=0 && nearest!=NULL){
    printf("FOUND NEAREST NEIGHBOR ");
    printVector(nearest);
    printf("WITH DISTANCE =  %f\n",nearestDist);
  }else{
    printf("- DID NOT FIND NEAREST NEIGHBOR\n");
  }
  printf("Checked: %d  vectors\n",searched);
}

void searchForHammingDistanceKNN(HyperCube hc,Vector v,int *v_index,int hammingDist,int startFrom,Vector *nearest,double *nearestDist,int *numOfSearched,int maxToSearch,int knn){
  if(hammingDist<=0){
    int new_index = binaryArrayToDecimal(v_index,k);
    printf("** HAMMING INDEX =%d\n",new_index);
    htKFindNearestNeighborsCube(hc->hypercube,new_index,v,nearest,nearestDist,d,knn,numOfSearched,maxToSearch);
    return;
  }
  for(int i=startFrom;i<k;i++){
      v_index[i] = v_index[i]^1;
      searchForHammingDistanceKNN(hc,v,v_index,hammingDist-1,i+1,nearest,nearestDist,numOfSearched,maxToSearch,knn);
      // search
      v_index[i] = v_index[i]^1;
      if((*numOfSearched)>=maxToSearch){
        break;
      }
  }
}

void kNearestNeigbors(HyperCube hc,Vector q,int knn,int hammingDist,int m){
  printf("ABOUT TO SEARCH %d NEAREST NEIGHBORS FOR : ",knn);
  printVector(q);
  Vector nearest[knn];
  double knearestDists[knn];
  for (int i = 0; i < knn; i++){
    knearestDists[i]=-1;
    nearest[i]=NULL;
  }

  int index[k];
  int searched = 0;
  for(int i=0;i<k;i++){
    int h_result = computeH(hc->h_functions[i],q);
    int f_result = computeF(hc->f_funs[i],h_result);
    index[i] = f_result;
  }
  int index_decimal = binaryArrayToDecimal(index,k);
  printf("** INITIAL INDEX =%d\n",index_decimal);
  htKFindNearestNeighborsCube(hc->hypercube,index_decimal,q,nearest,knearestDists,d,knn,&searched,m);

  for(int i=1;i<=hammingDist;i++){
    if(searched>=m){
      break;
    }
    printf("--------*********--------------\n");
    searchForHammingDistanceKNN(hc,q,index,i,0,nearest,knearestDists,&searched,m,knn);
  }

  int flag=1;
  for (int i = knn-1; i >= 0; i--){
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



void searchForHammingDistanceRadius(HyperCube hc,Vector v,int *v_index,int hammingDist,int startFrom,HashTable vecsInRadius,int *numOfSearched,int maxToSearch,int radius){
  if(hammingDist<=0){
    int new_index = binaryArrayToDecimal(v_index,k);
    printf("** HAMMING INDEX =%d\n",new_index);
    htFindNeighborsInRadiusCube(hc->hypercube,new_index,vecsInRadius,v,d,radius,numOfSearched,maxToSearch);
    return;
  }
  for(int i=startFrom;i<k;i++){
      v_index[i] = v_index[i]^1;
      searchForHammingDistanceRadius(hc,v,v_index,hammingDist-1,i+1,vecsInRadius,numOfSearched,maxToSearch,radius);
      // search
      v_index[i] = v_index[i]^1;
      if((*numOfSearched)>=maxToSearch){
        break;
      }
  }
}


void radiusNeigbor(HyperCube hc,Vector q,double radius,int hammingDist,int m){
  printf("ABOUT TO SEARCH FOR NEIGHBORS INSIDE RANGE : %f\n",radius);
  HashTable vecsInRadius = htInitialize(100); // TODO: CHANGE SIZE

  int index[k];
  int searched = 0;
  for(int i=0;i<k;i++){
    int h_result = computeH(hc->h_functions[i],q);
    int f_result = computeF(hc->f_funs[i],h_result);
    index[i] = f_result;
  }
  int index_decimal = binaryArrayToDecimal(index,k);
  printf("** INITIAL INDEX =%d\n",index_decimal);
  htFindNeighborsInRadiusCube(hc->hypercube,index_decimal,vecsInRadius,q,d,radius,&searched,m);

  for(int i=1;i<=hammingDist;i++){
    if(searched>=m){
      break;
    }
    printf("--------*********--------------\n");
    searchForHammingDistanceRadius(hc,q,index,i,0,vecsInRadius,&searched,m,radius);
  }

  htRangePrint(vecsInRadius,q,d);

  htDelete(vecsInRadius,0);
}












///
