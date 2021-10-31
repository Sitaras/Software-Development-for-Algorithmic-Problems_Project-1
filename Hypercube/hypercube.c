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
// extern int k;
extern int new_dimension;
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

void generateH_Cube(h_function *hfun){
  hfun->v=malloc(d*sizeof(double));
  for(int i=0;i<d;i++){
    hfun->v[i] = normalRandom();
  }
  hfun->t=uniform_distribution(0,w);
}

void destroyH_Cube(h_function h){
  free(h.v);
}

int computeH_Cube(h_function hfun,Vector vector){
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
  printf("NEW DIMENSION OF HYPERCUBE (d') = %d\n",new_dimension);
  HyperCube hc = malloc(sizeof(cubeNode));
  hc->h_functions = malloc(new_dimension*sizeof(h_function));
  for(int i=0;i<new_dimension;i++){
     generateH_Cube(&(hc->h_functions[i]));
  }
  hc->f_funs = malloc(new_dimension*sizeof(HashMap));
  for(int i=0;i<new_dimension;i++){
    hc->f_funs[i] = hmCreate(100);  // starting size is 100, hashMap is resizable
  }
  hc->hypercube = htInitialize((int)pow(2,new_dimension));  // 2^k buckets
  return hc;
}

void insertToHyperCube(HyperCube hc,Vector v){
  int index=0;
  for(int i=0;i<new_dimension;i++){
    int h_result = computeH_Cube(hc->h_functions[i],v);
    int f_result = computeF(hc->f_funs[i],h_result);
    index *=10;
    index += f_result;
  }
  // printf("INDEX = %d\n",index);
  int decimal_index = binaryToDecimal(index); // "hashTable key"
  htInsert(hc->hypercube,v,decimal_index,-1);
}

void printHyperCube(HyperCube hc){
    printf("-------- HyperCube --------\n");
    htPrint(hc->hypercube);
    printf("--------------------------\n");
}

void deleteHyperCube(HyperCube hc){
  for(int i=0;i<new_dimension;i++){
    destroyH_Cube(hc->h_functions[i]);
    hmDestroy(hc->f_funs[i]);
  }
  free(hc->f_funs);
  free(hc->h_functions);
  htDelete(hc->hypercube,1);
  free(hc);
}


void searchForHammingDistance(HyperCube hc,Vector v,int *v_index,int hammingDist,int startFrom,Vector *nearest,double *nearestDist,int *numOfSearched,int maxToSearch,int *nodesSearched,int probes){
  if(hammingDist<=0){
    int new_index = binaryArrayToDecimal(v_index,new_dimension);
    htFindNearestNeighborCube(hc->hypercube,new_index,v,nearest,nearestDist,d,numOfSearched,maxToSearch);
    (*nodesSearched)++;
    return;
  }
  for(int i=startFrom;i<new_dimension;i++){
      v_index[i] = v_index[i]^1;
      searchForHammingDistance(hc,v,v_index,hammingDist-1,i+1,nearest,nearestDist,numOfSearched,maxToSearch,nodesSearched,probes);
      // search
      v_index[i] = v_index[i]^1;
      if((*numOfSearched)>=maxToSearch || (*nodesSearched)>=probes){
        break;
      }
  }
}

void nearestNeigborHypercube(HyperCube hc,Vector q,int hammingDist,int m,FILE *fptr){
  printf("ABOUT TO SEARCH NEAREST NEIGHBOR FOR : ");
  fprintf(fptr,"ABOUT TO SEARCH NEAREST NEIGHBOR FOR : ");
  printVector(q);
  printVectorInFile(q,fptr);
  Vector nearest=NULL;
  double nearestDist=-1;
  int index[new_dimension];
  int searched = 0;
  for(int i=0;i<new_dimension;i++){
    int h_result = computeH_Cube(hc->h_functions[i],q);
    int f_result = computeF(hc->f_funs[i],h_result);
    index[i] = f_result;
  }
  int index_decimal = binaryArrayToDecimal(index,new_dimension);
  htFindNearestNeighborCube(hc->hypercube,index_decimal,q,&nearest,&nearestDist,d,&searched,m);

  int nodesSearched = 0;

  for(int i=1;;i++){
    if(searched>=m || nodesSearched>=hammingDist){
      break;
    }
    searchForHammingDistance(hc,q,index,i,0,&nearest,&nearestDist,&searched,m,&nodesSearched,hammingDist);
  }

  if(nearestDist>=0 && nearest!=NULL){
    printf("FOUND NEAREST NEIGHBOR ");
    fprintf(fptr,"FOUND NEAREST NEIGHBOR ");
    printVector(nearest);
    printf("WITH DISTANCE =  %f\n",nearestDist);
    printf("WITH DISTANCE =  %f\n",nearestDist);
    fprintf(fptr,"WITH DISTANCE =  %f\n",nearestDist);
  }else{
    fprintf(fptr,"- DID NOT FIND NEAREST NEIGHBOR\n");
    printf("- DID NOT FIND NEAREST NEIGHBOR\n");
  }
  // fprintf(fptr,"Checked: %d  vectors\n",searched);
  // printf("Checked: %d  vectors\n",searched);
  // fprintf(fptr,"Checked: %d  hypercube nodes\n",nodesSearched);
  // printf("Checked: %d  hypercube nodes\n",nodesSearched);
}

void searchForHammingDistanceKNN(HyperCube hc,Vector v,int *v_index,int hammingDist,int startFrom,Vector *nearest,double *nearestDist,int *numOfSearched,int maxToSearch,int knn,int *nodesSearched,int probes){
  if(hammingDist<=0){
    int new_index = binaryArrayToDecimal(v_index,new_dimension);
    htKFindNearestNeighborsCube(hc->hypercube,new_index,v,nearest,nearestDist,d,knn,numOfSearched,maxToSearch);
    (*nodesSearched)++;
    return;
  }
  for(int i=startFrom;i<new_dimension;i++){
      v_index[i] = v_index[i]^1;
      searchForHammingDistanceKNN(hc,v,v_index,hammingDist-1,i+1,nearest,nearestDist,numOfSearched,maxToSearch,knn,nodesSearched,probes);
      // search
      v_index[i] = v_index[i]^1;
      if((*numOfSearched)>=maxToSearch || (*nodesSearched)>=probes){
        break;
      }
  }
}

void kNearestNeigborsHypercube(HyperCube hc,Vector q,int knn,int hammingDist,int m,double *knearestTrueDists,FILE *fptr){
  // printf("ABOUT TO SEARCH %d NEAREST NEIGHBORS FOR : ",knn);
  // fprintf(fptr,"ABOUT TO SEARCH %d NEAREST NEIGHBORS FOR : ",knn);
  // printVector(q);
  // printVectorInFile(q,fptr);
  Vector nearest[knn];
  double knearestDists[knn];
  for (int i = 0; i < knn; i++){
    knearestDists[i]=-1;
    nearest[i]=NULL;
  }

  int index[new_dimension];
  int searched = 0;
  for(int i=0;i<new_dimension;i++){
    int h_result = computeH_Cube(hc->h_functions[i],q);
    int f_result = computeF(hc->f_funs[i],h_result);
    index[i] = f_result;
  }
  int index_decimal = binaryArrayToDecimal(index,new_dimension);
  // printf("** INITIAL INDEX =%d\n",index_decimal);
  htKFindNearestNeighborsCube(hc->hypercube,index_decimal,q,nearest,knearestDists,d,knn,&searched,m);

  int nodesSearched = 0;
  for(int i=1;;i++){
    if(searched>=m || nodesSearched>=hammingDist){
      break;
    }
    // printf("--------*********--------------\n");
    searchForHammingDistanceKNN(hc,q,index,i,0,nearest,knearestDists,&searched,m,knn,&nodesSearched,hammingDist);
  }

  int flag=1;
  for (int i = knn-1; i >= 0; i--){
    if (knearestDists[i] >= 0 && nearest[i] != NULL){
      // printf("Nearest neighbor-%d: ",knn-i);
      fprintf(fptr,"Nearest neighbor-%d: ",knn-i);
      // printVectorId(nearest[i]);
      printVectorIdInFile(nearest[i],fptr);
      // printf("distanceLSH: %f\n", knearestDists[i]);
      // printf("distanceTrue: %f\n", knearestTrueDists[i]);
      // printf("- %d\n",i);
      fprintf(fptr,"distanceHypercube: %f\n", knearestDists[i]);
      fprintf(fptr,"distanceTrue: %f\n", knearestTrueDists[i]);
      flag=0;
    }
  }
  if(flag){
    // printf("- DID NOT FIND NEAREST NEIGHBOR\n");
    fprintf(fptr,"- DID NOT FIND NEAREST NEIGHBOR\n");
  }
  // printf("Checked: %d  vectors\n",searched);
  // printf("Checked: %d  hypercube nodes\n",nodesSearched);
}



void searchForHammingDistanceRadius(HyperCube hc,Vector v,int *v_index,int hammingDist,int startFrom,HashTable vecsInRadius,int *numOfSearched,int maxToSearch,int radius,int *nodesSearched,int probes){
  if(hammingDist<=0){
    int new_index = binaryArrayToDecimal(v_index,new_dimension);
    // printf("** HAMMING INDEX =%d\n",new_index);
    htFindNeighborsInRadiusCube(hc->hypercube,new_index,vecsInRadius,v,d,radius,numOfSearched,maxToSearch);
    (*nodesSearched)++;
    return;
  }
  for(int i=startFrom;i<new_dimension;i++){
      v_index[i] = v_index[i]^1;
      searchForHammingDistanceRadius(hc,v,v_index,hammingDist-1,i+1,vecsInRadius,numOfSearched,maxToSearch,radius,nodesSearched,probes);
      // search
      v_index[i] = v_index[i]^1;
      if((*numOfSearched)>=maxToSearch || (*nodesSearched)>=probes){
        break;
      }
  }
}


void radiusNeigborHypercube(HyperCube hc,Vector q,double radius,int hammingDist,int m,FILE *fptr){
  // printf("ABOUT TO SEARCH FOR NEIGHBORS INSIDE RANGE : %f\n",radius);
  // fprintf(fptr,"ABOUT TO SEARCH FOR NEIGHBORS INSIDE RANGE : %f\n",radius);

  int vecsInRadius_size = getNumberOfVectors(hc->hypercube);
  HashTable vecsInRadius = htInitialize(vecsInRadius_size/8); // TODO: CHANGE SIZE
  int index[new_dimension];
  int searched = 0;
  for(int i=0;i<new_dimension;i++){
    int h_result = computeH_Cube(hc->h_functions[i],q);
    int f_result = computeF(hc->f_funs[i],h_result);
    index[i] = f_result;
  }
  int index_decimal = binaryArrayToDecimal(index,new_dimension);
  // printf("** INITIAL INDEX =%d\n",index_decimal);
  htFindNeighborsInRadiusCube(hc->hypercube,index_decimal,vecsInRadius,q,d,radius,&searched,m);

  int nodesSearched = 0;
  for(int i=1;;i++){
    if(searched>=m || nodesSearched>=hammingDist){
      break;
    }
    // printf("--------*********--------------\n");
    searchForHammingDistanceRadius(hc,q,index,i,0,vecsInRadius,&searched,m,radius,&nodesSearched,hammingDist);
  }

  htRangePrint(vecsInRadius,q,d,fptr);

  htDelete(vecsInRadius,0);
  // fprintf(fptr,"Checked: %d  vectors\n",searched);
  // printf("Checked: %d  vectors\n",searched);
  // fprintf(fptr,"Checked: %d  hypercube nodes\n",nodesSearched);
  // printf("Checked: %d  hypercube nodes\n",nodesSearched);
}





void searchForHammingDistanceRadiusClustering(HyperCube hc,Vector v,int *v_index,int hammingDist,int startFrom,HashTable vecsInRadius,int *numOfSearched,int maxToSearch,int radius,int *nodesSearched,int probes,int centroidIndex,List* confList,int *assignCounter,int iteration){
  if(hammingDist<=0){
    int new_index = binaryArrayToDecimal(v_index,new_dimension);
    htFindNeighborsInRadiusClusteringCube(hc->hypercube,new_index,centroidIndex,confList,vecsInRadius,v,d,radius,numOfSearched,maxToSearch,assignCounter,iteration);
    (*nodesSearched)++;
    return;
  }
  for(int i=startFrom;i<new_dimension;i++){
      v_index[i] = v_index[i]^1;
      searchForHammingDistanceRadiusClustering(hc,v,v_index,hammingDist-1,i+1,vecsInRadius,numOfSearched,maxToSearch,radius,nodesSearched,probes,centroidIndex,confList,assignCounter,iteration);
      // search
      v_index[i] = v_index[i]^1;
      if((*numOfSearched)>=maxToSearch || (*nodesSearched)>=probes){
        break;
      }
  }
}

void radiusNeigborHypercubeClustering(HyperCube hc,Vector q,HashTable vecsInRadius,double radius,int hammingDist,int m,int centroidIndex,List* confList,int *assignCounter,int iteration){
  printf("ABOUT TO SEARCH FOR NEIGHBORS INSIDE RANGE : %f\n",radius);
  // fprintf(fptr,"ABOUT TO SEARCH FOR NEIGHBORS INSIDE RANGE : %f\n",radius);
  // HashTable vecsInRadius = htInitialize(100); // TODO: CHANGE SIZE

  int index[new_dimension];
  int searched = 0;
  for(int i=0;i<new_dimension;i++){
    int h_result = computeH_Cube(hc->h_functions[i],q);
    int f_result = computeF(hc->f_funs[i],h_result);
    index[i] = f_result;
  }
  int index_decimal = binaryArrayToDecimal(index,new_dimension);
  // htFindNeighborsInRadiusCube(hc->hypercube,index_decimal,vecsInRadius,q,d,radius,&searched,m);
  htFindNeighborsInRadiusClusteringCube(hc->hypercube,index_decimal,centroidIndex,confList,vecsInRadius,q,d,radius,&searched,m,assignCounter,iteration);

  int nodesSearched = 0;
  for(int i=1;;i++){
    if(searched>=m || nodesSearched>=hammingDist){
      break;
    }
    searchForHammingDistanceRadiusClustering(hc,q,index,i,0,vecsInRadius,&searched,m,radius,&nodesSearched,hammingDist,centroidIndex,confList,assignCounter,iteration);
  }


  // fprintf(fptr,"Checked: %d  vectors\n",searched);
  // printf("Checked: %d  vectors\n",searched);
  // // fprintf(fptr,"Checked: %d  hypercube nodes\n",nodesSearched);
  // printf("Checked: %d  hypercube nodes\n",nodesSearched);
}












///
