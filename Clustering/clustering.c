#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include "../Vector/vector.h"
#include "../hashTable/hashTableList/hashTableList.h"
#include "../hashTable/hashTable.h"
#include "../LSH/lsh.h"
#include "../Hypercube/hypercube.h"
#include "./clusterHelpingFuns.h"
#include "./kmeansPlusPlus.h"

#define SQUARE(x) ((x)*(x))

#define TRUE 1
#define FALSE 0
#define MAX_RECENTER_ITERATIONS 30

extern int numOfVecs;
extern int d;
extern int hashTableSize;



void lloyds(Vector* clusters,Vector *oldClusters,Vector* vectors,List* clustersList,int numberOfVectors,int numOfClusters) {
  static int flag=0;
  if(flag) //skip it for the first time
    for(int i=0;i<numOfClusters;i++){
      Vector newCenter;
      if(clustersList[i]!=NULL){
        newCenter=listMeanOfCluster(clustersList[i],d);
      }
      else{
        newCenter=copyVector(oldClusters[i]);
      }
      listDelete(clustersList[i],0);
      clustersList[i] = NULL;

      clusters[i]=newCenter;
    }

  for(int i=0;i<numberOfVectors;i++){
    int closestCentroid = findClosestCentroid(vectors[i],clusters,numOfClusters);
    clustersList[closestCentroid] = listInsert(clustersList[closestCentroid],vectors[i],d);
  }
  flag=1;
}

void reverseAssignmentLSH(LSH lsh,Vector *vectors,Vector *clusters,Vector *oldClusters,HashTable *clustersHt,int numOfClusters,int iteration){
  static int flag=0;
  printf("ITERATION WITH LSH %d\n",iteration);
  if(flag) //skip it for the first time
    for(int i=0;i<numOfClusters;i++){

      Vector newCenter = htMeanOfCluster(clustersHt[i],d);
      if(newCenter==NULL){
        newCenter=copyVector(oldClusters[i]);
      }

      htDelete(clustersHt[i],0);
      clustersHt[i] = htInitialize(50);

      clusters[i]=newCenter;
    }
  double radius=DBL_MAX;
  minDistbetweenCentroids(clusters,numOfClusters,&radius);
  radius/=2;
  int assignCounter = 0;
  while((double)assignCounter<(0.8*numOfVecs)){ // stop when the 80% of vectors has a cluster (do it with global var)
    printf("ABOUT TO SEARCH FOR NEIGHBORS INSIDE RANGE : %f\n",radius);
    List confList=initializeList();
    for(int i=0;i<numOfClusters;i++){
      radiusNeigborClustering(lsh,clusters[i],radius,clustersHt[i],i,&confList,&assignCounter,iteration);
    }
    listSolveRangeConflicts(confList,clustersHt,clusters,numOfClusters,d);
    printf("---- ASSINGED ITEMS = %d\n",assignCounter);
    // manage the conflicts
    if(confList==NULL)
      printf("- NO CONFLICTS FOUND\n");
    listDelete(confList,0);
    radius*=2;
  }
  int remainderCounter = 0;
  if(assignCounter<numOfVecs){
    for(int i=0;i<numOfVecs;i++){
      if(assignedToCluster(vectors[i]) && (getAssignedIteration(vectors[i])==iteration)){
        continue;
      }
      int closestCentroid = findClosestCentroid(vectors[i],clusters,numOfClusters);
      htRangeInsert(clustersHt[closestCentroid],vectors[i],-1,d);
      setAssignedCluster(vectors[i],closestCentroid);
      setAssignedIteration(vectors[i],iteration);
      setAssignedAtRadius(vectors[i],radius);
      remainderCounter++;
    }
  }
  printf("---- ASSINGED REMAINING ITEMS = %d\n",remainderCounter);
  flag=1;
}


void clusteringLloyds(List vecList,int numOfClusters){
  Vector *vectors;
  Vector *clusters;
  Vector *oldClusters = NULL;
  double *props;

  vectors = transformListToArray(vecList,numOfVecs);
  clusters = malloc(numOfClusters*sizeof(Vector));
  oldClusters = malloc(numOfClusters*sizeof(Vector));
  for(int i=0;i<numOfClusters;i++){
    clusters[i] = NULL;
    oldClusters[i] = NULL;
  }
  props = calloc(numOfVecs,sizeof(double));


  kmeansplusplus(vectors,numOfClusters,clusters,props);

  int firstIter = TRUE;

  //lloyds
  List *clustersList=malloc(numOfClusters*sizeof(List *));
  for(int i=0;i<numOfClusters;i++){
    clustersList[i]=initializeList();
  }
  int count=0;
  while((count<2) || !centroidsCovnerge(clusters,oldClusters,numOfClusters,d)){
  // while(firstIter || count<20){
    printf("ITER %d\n",count);
    count++;
    if(!firstIter){
      Vector *temp = oldClusters;
      oldClusters=clusters;
      clusters = temp;
      for(int i=0;i<numOfClusters;i++){
        if(clusters[i]!=NULL){
          deleteVector(clusters[i]);
          clusters[i] = NULL;
        }
      }
    }

    //
    lloyds(clusters,oldClusters,vectors,clustersList,numOfVecs,numOfClusters);

    firstIter=FALSE;
  }


  //
  for(int i=0;i<numOfClusters;i++){
    printf("- CLUSTER :%d\n",i);
    printVector(clusters[i]);
    // printf("- CLUSTER LIST\n");
    // listPrint(clustersList[i]);
  }

  // free clusters
  for(int i=0;i<numOfClusters;i++){
    listDelete(clustersList[i],0);
    deleteVector(oldClusters[i]);
    deleteVector(clusters[i]);
  }
  free(clusters);
  free(props);
  free(vectors);
  free(clustersList);
  free(oldClusters);
}

void clusteringLSH(List vecList,int numOfClusters){
  Vector *vectors;
  Vector *clusters;
  Vector *oldClusters = NULL;
  double *props;
  HashTable *clustersHt=malloc(numOfClusters*sizeof(HashTable *));
  for(int i=0;i<numOfClusters;i++){
    clustersHt[i]= htInitialize(50); // TODO: CHANGE SIZE
  }
  vectors = transformListToArray(vecList,numOfVecs);
  clusters = malloc(numOfClusters*sizeof(Vector));
  oldClusters = malloc(numOfClusters*sizeof(Vector));
  for(int i=0;i<numOfClusters;i++){
    clusters[i]=NULL;
    oldClusters[i]=NULL;
  }
  props = calloc(numOfVecs,sizeof(double));
  kmeansplusplus(vectors,numOfClusters,clusters,props);
  for(int i=0;i<numOfClusters;i++){
    printf("- CLUSTER :%d\n",i);
    printVector(clusters[i]);
  }

  hashTableSize=numOfVecs/8;
  LSH lsh = initializeLSH(6);
  for(int i=0;i<numOfVecs;i++){
    initializeClusterInfo(vectors[i]);
    insertToLSH(lsh,vectors[i]);
  }

  int firstIterLSH = TRUE;
  int countLSH=0;
  while((countLSH<2) || !centroidsCovnerge(clusters,oldClusters,numOfClusters,d)){
    if(countLSH==MAX_RECENTER_ITERATIONS)
      break;
  // while(firstIter || count<20){
    countLSH++;
    if(!firstIterLSH){
      Vector *temp = oldClusters;
      oldClusters=clusters;
      clusters = temp;
      for(int i=0;i<numOfClusters;i++){
        if(clusters[i]!=NULL){
          deleteVector(clusters[i]);
          clusters[i] = NULL;
        }
      }
    }

    //
    reverseAssignmentLSH(lsh,vectors,clusters,oldClusters,clustersHt,numOfClusters,countLSH);

    firstIterLSH=FALSE;
  }

  for(int i=0;i<numOfClusters;i++){
    printf("- CLUSTER :%d\n",i);
    printVector(clusters[i]);
  }

  for(int i=0;i<numOfClusters;i++){
    if(oldClusters[i]!=NULL)
      deleteVector(oldClusters[i]);
    if(clusters[i]!=NULL)
      deleteVector(clusters[i]);
    htDelete(clustersHt[i],0);
  }

  free(props);
  free(vectors);
  free(oldClusters);
  free(clustersHt);
  free(clusters);
  destroyLSH(lsh);
}

void reverseAssignmentHypercube(HyperCube cube,Vector *vectors,Vector *clusters,Vector *oldClusters,HashTable *clustersHt,int numOfClusters,int iteration,int m,int probes){
  static int flag=0;
  printf("ITERATION WITH LSH %d\n",iteration);
  if(flag) //skip it for the first time
    for(int i=0;i<numOfClusters;i++){

      Vector newCenter = htMeanOfCluster(clustersHt[i],d);
      if(newCenter==NULL){
        newCenter=copyVector(oldClusters[i]);
      }

      htDelete(clustersHt[i],0);
      clustersHt[i] = htInitialize(50);

      clusters[i]=newCenter;
    }
  double radius=DBL_MAX;
  minDistbetweenCentroids(clusters,numOfClusters,&radius);
  radius/=2;
  int assignCounter = 0;
  int previousAssigns = -1;
  int loopCounter = 0;
  while((double)assignCounter<(0.8*numOfVecs) && loopCounter<MAX_RECENTER_ITERATIONS){ // stop when the 80% of vectors has a cluster (do it with global var)
    if(assignCounter==previousAssigns && assignCounter!=0){
      break;
    }
    previousAssigns = assignCounter;
    printf("ABOUT TO SEARCH FOR NEIGHBORS INSIDE RANGE : %f\n",radius);
    List confList=initializeList();
    for(int i=0;i<numOfClusters;i++){
      radiusNeigborHypercubeClustering(cube,clusters[i],clustersHt[i],radius,probes,m,i,&confList,&assignCounter,iteration);
    }
    listSolveRangeConflicts(confList,clustersHt,clusters,numOfClusters,d);
    printf("---- ASSINGED ITEMS = %d\n",assignCounter);
    // manage the conflicts
    if(confList==NULL)
      printf("- NO CONFLICTS FOUND\n");
    listDelete(confList,0);
    radius*=2;
    loopCounter++;
  }
  int remainderCounter = 0;
  if(assignCounter<numOfVecs){
    for(int i=0;i<numOfVecs;i++){
      if(assignedToCluster(vectors[i]) && (getAssignedIteration(vectors[i])==iteration)){
        continue;
      }
      int closestCentroid = findClosestCentroid(vectors[i],clusters,numOfClusters);
      htRangeInsert(clustersHt[closestCentroid],vectors[i],-1,d);
      setAssignedCluster(vectors[i],closestCentroid);
      setAssignedIteration(vectors[i],iteration);
      setAssignedAtRadius(vectors[i],radius);
      remainderCounter++;
    }
  }
  printf("---- ASSINGED REMAINING ITEMS = %d\n",remainderCounter);
  flag=1;
}


void clusteringHypercube(List vecList,int numOfClusters,int m,int probes){
  Vector *vectors;
  Vector *clusters;
  Vector *oldClusters = NULL;
  double *props;
  HashTable *clustersHt=malloc(numOfClusters*sizeof(HashTable *));
  for(int i=0;i<numOfClusters;i++){
    clustersHt[i]= htInitialize(50); // TODO: CHANGE SIZE
  }
  vectors = transformListToArray(vecList,numOfVecs);
  clusters = malloc(numOfClusters*sizeof(Vector));
  oldClusters = malloc(numOfClusters*sizeof(Vector));
  for(int i=0;i<numOfClusters;i++){
    clusters[i]=NULL;
    oldClusters[i]=NULL;
  }
  props = calloc(numOfVecs,sizeof(double));
  kmeansplusplus(vectors,numOfClusters,clusters,props);
  for(int i=0;i<numOfClusters;i++){
    printf("- CLUSTER :%d\n",i);
    printVector(clusters[i]);
  }

  hashTableSize=numOfVecs/8;
  HyperCube cube = initializeHyperCube();
  for(int i=0;i<numOfVecs;i++){
    initializeClusterInfo(vectors[i]);
    insertToHyperCube(cube,vectors[i]);
  }

  int firstIterLSH = TRUE;
  int countLSH=0;
  while((countLSH<2) || !centroidsCovnerge(clusters,oldClusters,numOfClusters,d)){
    if(countLSH==MAX_RECENTER_ITERATIONS)
      break;
  // while(firstIter || count<20){
    countLSH++;
    if(!firstIterLSH){
      Vector *temp = oldClusters;
      oldClusters=clusters;
      clusters = temp;
      for(int i=0;i<numOfClusters;i++){
        if(clusters[i]!=NULL){
          deleteVector(clusters[i]);
          clusters[i] = NULL;
        }
      }
    }

    //
    reverseAssignmentHypercube(cube,vectors,clusters,oldClusters,clustersHt,numOfClusters,countLSH,m,probes);

    firstIterLSH=FALSE;
  }
}

void clustering(List vecList,int numOfClusters,int m,int probes){

  // clusteringLloyds(vecList,numOfClusters);

  printf("\n==============================2\n");

  // clusteringLSH(vecList,numOfClusters);

  clusteringHypercube(vecList,numOfClusters,m,probes);


  //Reverse approach

}
