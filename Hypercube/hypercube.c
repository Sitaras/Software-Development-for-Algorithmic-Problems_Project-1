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

int binaryToDecimal(long long int n){
    long long int num = n;
    int dec_value = 0;
    int base = 1; // Initializing base value to 1, i.e 2^0
    long long int temp = num;

    while (temp){
      int last_digit = temp % 10;
      temp = temp / 10;
      dec_value += last_digit * base;
      base = base * 2;
      printf("dec_value = %d\n",dec_value );
    }

    return dec_value;
}

int binaryArrayToDecimal(int s[],int size){
    int n = 0;
    int i;
    for (i = 0; i < size; ++i){
      n <<= 1;
      n += s[i];
    }
    return n;
}

/* H FUNCTIONS */

void generateH_Cube(h_function *hfun){
  // generate v vector coordinates of h function, v ∼ N (0, 1)^d
  hfun->v=malloc(d*sizeof(double));
  for(int i=0;i<d;i++){
    hfun->v[i] = normalRandom();
  }
  // pick t variable uniformly ∈ R [0, w)
  double temp = uniform_distribution(0,w);
  hfun->t=temp;
}

void destroyH_Cube(h_function h){
  free(h.v);
}

int computeH_Cube(h_function hfun,Vector vector){
  // compute the dot product of the given vector with the v vector of h function (p · v)
  double pv = dot_product(hfun.v,getCoords(vector),d);
  // finally calculate the value of h function
  double temp = (double) (pv+hfun.t)/(double)w;
  return floor(temp);
}

/* F Functions */

int flipCoin(){
  return rand()%2;
}

int computeF(HashMap f_fun,Key key){
  // every f function is represented as a hashMap (the common dictionary in Python)
  // a f function for each function h returns a specific value of 0 or 1.
  // The item of a hash map is randomly generated (flipCoin) and stored in the corresponding bucket (if the key doesn't already exist).
  // The value of the h function is the key of the a hash map .
  // The item of the hash map is a random value (bit) 0 or 1 .
  // hash function of hash map is: The value of the h function mod hash_map_size
  int bit = flipCoin();
  Record r = hmSearchOrInsert(f_fun,key,bit);
  return getValue(r);
}


HyperCube initializeHyperCube(){
  // allocate and initialize the hyper cube node
  HyperCube hc = malloc(sizeof(cubeNode));
  // allocate and generate h functions based on the given dimension
  hc->h_functions = malloc(new_dimension*sizeof(h_function));
  for(int i=0;i<new_dimension;i++){
     generateH_Cube(&(hc->h_functions[i]));
  }
  // allocate and generate f functions based on the given dimension
  hc->f_funs = malloc(new_dimension*sizeof(HashMap));
  for(int i=0;i<new_dimension;i++){
    hc->f_funs[i] = hmCreate(100);  // starting size is 100, hashMap is resizable
  }
  // finally allocate and initialize tha hash table that hyper cube needs
  hc->hypercube = htInitialize((int)pow(2,new_dimension));  // 2^k buckets
  return hc;
}

void insertToHyperCube(HyperCube hc,Vector v){
  // insert the given vector in hyper cube (at the corresponding bucket of the hash table)
  // long long int index=0;
  int indexArray[new_dimension];
  for(int i=0;i<new_dimension;i++){ // form/find the corresponding index of the hypercube (hash table) that the given vector will be inserted
    int h_result = computeH_Cube(hc->h_functions[i],v); // compute the h function's value based the given v vector
    int f_result = computeF(hc->f_funs[i],h_result); // compute the f function's value based on the previous value that return the h function
    // f returns a bit, 0 or 1, take this bit and append it to the index to form that for the hypercube
    indexArray[i]=f_result;
    // and continue for the next dimension
  }
  // index formed, convert it to decimal
  // int decimal_index = binaryToDecimal(index); // hash Table key
  int decimal_index = binaryArrayToDecimal(indexArray,new_dimension);  // hash Table key
  // finally insert the given vector at the hash table
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

void nearestNeigborHypercube(HyperCube hc,Vector q,int hammingDist,int m,double *trueDist,FILE *fptr){
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
    if(searched>=m || nodesSearched>=hammingDist || i>new_dimension){
      break;
    }
    searchForHammingDistance(hc,q,index,i,0,&nearest,&nearestDist,&searched,m,&nodesSearched,hammingDist);
  }

  if(nearestDist>=0 && nearest!=NULL){
    fprintf(fptr,"Nearest neighbor-1: ");
    printVectorIdInFile(nearest,fptr);
    fprintf(fptr,"distanceHypercube: %f\n",nearestDist);
    fprintf(fptr,"distanceTrue: %f\n", *trueDist);
  }else{
    fprintf(fptr,"- DID NOT FIND NEAREST NEIGHBOR\n");
  }
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
  // find the k nearest neighbours of the given vector q with the help of hypercube
  Vector nearest[knn]; // an array to save the  k nearest neighbours (vectors)
  double knearestDists[knn]; // an array to save the distances of k nearest neighbours
  for (int i = 0; i < knn; i++){
    knearestDists[i]=-1;
    nearest[i]=NULL;
  }

  int index[new_dimension];
  int searched = 0;
  for(int i=0;i<new_dimension;i++){ // form/find the corresponding index of the hypercube (hash table) that the given vector will be inserted
    int h_result = computeH_Cube(hc->h_functions[i],q); // compute the h function's value based the given v vector
    int f_result = computeF(hc->f_funs[i],h_result); // compute the f function's value based on the previous value that return the h function
    // f returns a bit, 0 or 1, take this bit and append it to the index to form that for the hypercube
    index[i] = f_result;
    // and continue for the next dimension
  }
  // index formed, convert it to decimal
  int index_decimal = binaryArrayToDecimal(index,new_dimension);
  // finally search k nearest neighbors for the given vector at the corresponding bucket of the hash table
  htKFindNearestNeighborsCube(hc->hypercube,index_decimal,q,nearest,knearestDists,d,knn,&searched,m);

  int nodesSearched = 0;
  for(int i=1;;i++){
    if(searched>=m || nodesSearched>=hammingDist || i>new_dimension){
      break;
    }
    searchForHammingDistanceKNN(hc,q,index,i,0,nearest,knearestDists,&searched,m,knn,&nodesSearched,hammingDist);
  }

  int flag=1;
  for (int i = knn-1; i >= 0; i--){
    if (knearestDists[i] >= 0 && nearest[i] != NULL){
      // check if k nearest neighbor of the given vector q found or not
      fprintf(fptr,"Nearest neighbor-%d: ",knn-i);
      printVectorIdInFile(nearest[i],fptr);
      fprintf(fptr,"distanceHypercube: %f\n", knearestDists[i]);
      fprintf(fptr,"distanceTrue: %f\n", knearestTrueDists[i]);
      flag=0;
    }
  }
  if(flag){
    fprintf(fptr,"- DID NOT FIND NEAREST NEIGHBOR\n");
  }
  fflush(fptr);
}



void searchForHammingDistanceRadius(HyperCube hc,Vector v,int *v_index,int hammingDist,int startFrom,HashTable vecsInRadius,int *numOfSearched,int maxToSearch,int radius,int *nodesSearched,int probes){
  if(hammingDist<=0){
    int new_index = binaryArrayToDecimal(v_index,new_dimension);
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


void radiusNeigborsHypercube(HyperCube hc,Vector q,double radius,int hammingDist,int m,FILE *fptr){
  // find the neighbours of the given vector q inside the given radius with the help of hypercube
  // store adjacent vectors in a hash table
  int vecsInRadius_size = getNumberOfVectors(hc->hypercube);
  HashTable vecsInRadius = htInitialize(vecsInRadius_size/8); // hash table to store the adjacent vectors of the given vector q
  int index[new_dimension];
  int searched = 0;
  for(int i=0;i<new_dimension;i++){ // form/find the corresponding index of the hypercube (hash table) that the given vector will be inserted
    int h_result = computeH_Cube(hc->h_functions[i],q); // compute the h function's value based the given v vector
    int f_result = computeF(hc->f_funs[i],h_result); // compute the f function's value based on the previous value that return the h function
    // f returns a bit, 0 or 1, take this bit and append it to the index to form that for the hypercube
    index[i] = f_result;
    // and continue for the next dimension
  }
  // index formed, convert it to decimal
  int index_decimal = binaryArrayToDecimal(index,new_dimension);
  // finally search for the neighbors of the given vector that are inside at the given radius at the corresponding bucket of the hash table
  htFindNeighborsInRadiusCube(hc->hypercube,index_decimal,vecsInRadius,q,d,radius,&searched,m);

  int nodesSearched = 0;
  for(int i=1;;i++){
    if(searched>=m || nodesSearched>=hammingDist || i>new_dimension){
      break;
    }
    searchForHammingDistanceRadius(hc,q,index,i,0,vecsInRadius,&searched,m,radius,&nodesSearched,hammingDist);
  }

  htRangePrint(vecsInRadius,q,d,fptr);

  htDelete(vecsInRadius,0);
}





void searchForHammingDistanceRadiusClustering(HyperCube hc,Vector v,int *v_index,int hammingDist,int startFrom,HashTable vecsInRadius,int *numOfSearched,int maxToSearch,double radius,int *nodesSearched,int probes,int centroidIndex,List* confList,int *assignCounter,int iteration){
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
  // based on the given centroids find the clusters that the given vectors belong with the help of hypercube (this function used for the "reverseAssignmentHypercube")
  // the clusters are represented by hash tables
  int index[new_dimension];
  int searched = 0;
  for(int i=0;i<new_dimension;i++){ // form/find the corresponding index of the hypercube (hash table) that the given vector will be inserted
    int h_result = computeH_Cube(hc->h_functions[i],q);  // compute the h function's value based the given v vector
    int f_result = computeF(hc->f_funs[i],h_result); // compute the f function's value based on the previous value that return the h function
    // f returns a bit, 0 or 1, take this bit and append it to the index to form that for the hypercube
    index[i] = f_result;
    // and continue for the next dimension
  }
  // index formed, convert it to decimal
  int index_decimal = binaryArrayToDecimal(index,new_dimension);
  // finally go at the corresponding bucket of the hash table to assign the vectors in to the clusters
  htFindNeighborsInRadiusClusteringCube(hc->hypercube,index_decimal,centroidIndex,confList,vecsInRadius,q,d,radius,&searched,m,assignCounter,iteration);

  int nodesSearched = 0;
  for(int i=1;;i++){
    if(searched>=m || nodesSearched>=hammingDist || i>new_dimension){
      break;
    }
    searchForHammingDistanceRadiusClustering(hc,q,index,i,0,vecsInRadius,&searched,m,radius,&nodesSearched,hammingDist,centroidIndex,confList,assignCounter,iteration);
  }

}












///
