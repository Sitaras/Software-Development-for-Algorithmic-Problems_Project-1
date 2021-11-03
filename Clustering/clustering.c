#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <time.h>
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



void lloyds(Vector* clusters,Vector *oldClusters,Vector* vectors,List* clustersList,int numberOfVectors,int numOfClusters,int *vectorCount,int *firstTime) {
  if(*firstTime) //skip it for the first time
    for(int i=0;i<numOfClusters;i++){
      Vector newCenter;
      if(clustersList[i]!=NULL){
        newCenter=listMeanOfCluster(clustersList[i],d);
      }
      else{
        printVectorId(oldClusters[i]);
        newCenter=copyVector(oldClusters[i]);
      }
      listDelete(clustersList[i],0);
      clustersList[i] = NULL;

      clusters[i]=newCenter;
    }

  for(int i=0;i<numberOfVectors;i++){
    int closestCentroid = findClosestCentroid(vectors[i],clusters,numOfClusters);
    vectorCount[closestCentroid] += 1;
    clustersList[closestCentroid] = listInsert(clustersList[closestCentroid],vectors[i],d);
  }
  *firstTime=1;
}

double *silhouetteLloyds(List *clustersList,Vector *clusters,int numOfClusters,int *vectorCount){
  double *silhouettes = calloc(sizeof(double),numOfClusters);
  for(int i=0;i<numOfClusters;i++){
    silhouettes[i] = silhouetteofClusterLloyds(clustersList,clusters,i,numOfClusters,vectorCount[i],d);
  }
  return silhouettes;
}

void clusteringLloyds(List vecList,int numOfClusters,FILE* fptr){
  Vector *vectors;
  Vector *clusters;
  Vector *oldClusters = NULL;
  double *props;
  int *vectorCount;

  vectors = transformListToArray(vecList,numOfVecs);
  clusters = malloc(numOfClusters*sizeof(Vector));
  oldClusters = malloc(numOfClusters*sizeof(Vector));

  for(int i=0;i<numOfClusters;i++){
    clusters[i] = NULL;
    oldClusters[i] = NULL;
  }
  props = calloc(numOfVecs,sizeof(double));
  vectorCount = calloc(numOfVecs,sizeof(int));


  kmeansplusplus(vectors,numOfClusters,clusters,props);

  int firstIter = TRUE;

  //lloyds
  List *clustersList=malloc(numOfClusters*sizeof(List *));
  for(int i=0;i<numOfClusters;i++){
    clustersList[i]=initializeList();
  }
  int count=0;
  int firstTime=0;
  while((count<2) || !centroidsConverge(clusters,oldClusters,numOfClusters,d)){
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
        vectorCount[i] = 0;
      }
    }

    //
    lloyds(clusters,oldClusters,vectors,clustersList,numOfVecs,numOfClusters,vectorCount,&firstTime);

    firstIter=FALSE;
  }

  for(int i=0;i<numOfClusters;i++){
    fprintf(fptr,"CLUSTER-%d {size: %d",i+1,vectorCount[i]);
    printVectorInFile(clusters[i],fptr);
    fprintf(fptr,"}\n");
  }

  printf("- COMPUTING SILHOUETTES FOR CLUSTERS\n");
  double * silhouettes = silhouetteLloyds(clustersList,clusters,numOfClusters,vectorCount);
  printf("- FINISHED COMPUTING SILHOUETTES\n");
  for(int i=0;i<numOfClusters;i++){
    printf("CLUSTER-%d { silhouette: %f }\n",i+1,silhouettes[i]);
  }

  // free clusters
  for(int i=0;i<numOfClusters;i++){
    listDelete(clustersList[i],0);
    deleteVector(oldClusters[i]);
    deleteVector(clusters[i]);
  }
  free(silhouettes);
  free(clusters);
  free(props);
  free(vectors);
  free(clustersList);
  free(oldClusters);
  free(vectorCount);
}

void reverseAssignmentLSH(LSH lsh,Vector *vectors,Vector *clusters,Vector *oldClusters,HashTable *clustersHt,int numOfClusters,int iteration,int *firstTime){
  printf("ITERATION WITH LSH %d\n",iteration);
  if(*firstTime) //skip it for the first time
    for(int i=0;i<numOfClusters;i++){

      Vector newCenter = htMeanOfCluster(clustersHt[i],d);
      if(newCenter==NULL){
        newCenter=copyVector(oldClusters[i]);
      }

      htDelete(clustersHt[i],0);
      clustersHt[i] = htInitialize(numOfVecs/(4*numOfClusters)); // TODO: CHANGE

      clusters[i]=newCenter;
    }
  double radius=DBL_MAX;
  minDistbetweenCentroids(clusters,numOfClusters,&radius);
  radius/=2;
  int assignCounter = 0;
  int previousAssigns = -1;
  int loopCounter = 0;
  while((double)assignCounter<(0.8*numOfVecs) && loopCounter<MAX_RECENTER_ITERATIONS){ // stop when the 80% of vectors has a cluster (do it with global var)
    if(assignCounter==previousAssigns && assignCounter!=0 && loopCounter>5){
      break;
    }
    previousAssigns = assignCounter;
    printf("ABOUT TO SEARCH FOR NEIGHBORS INSIDE RANGE : %f\n",radius);
    List confList=initializeList();
    for(int i=0;i<numOfClusters;i++){
      radiusNeigborsClustering(lsh,clusters[i],radius,clustersHt[i],i,&confList,&assignCounter,iteration);
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
  *firstTime=1;
}


double *silhouetteLSH_Hypercube(HashTable *clustersHt,Vector *clusters,int numOfClusters){
  double *silhouettes = calloc(sizeof(double),numOfClusters);
  for(int i=0;i<numOfClusters;i++){
    silhouettes[i] = silhouetteofClusterLSH(clustersHt,clusters,i,numOfClusters,d);
  }
  return silhouettes;
}

void clusteringLSH(List vecList,int numOfClusters,int l,FILE* fptr){
  Vector *vectors;
  Vector *clusters;
  Vector *oldClusters = NULL;
  double *props;
  HashTable *clustersHt=malloc(numOfClusters*sizeof(HashTable *));
  for(int i=0;i<numOfClusters;i++){
    clustersHt[i]= htInitialize(numOfVecs/(4*numOfClusters)); // TODO: CHANGE SIZE
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
  // for(int i=0;i<numOfClusters;i++){
  //   printf("- CLUSTER :%d\n",i);
  //   printVector(clusters[i]);
  // }

  hashTableSize=numOfVecs/16;
  LSH lsh = initializeLSH(l);
  for(int i=0;i<numOfVecs;i++){
    initializeClusterInfo(vectors[i]);
    insertToLSH(lsh,vectors[i]);
  }


  int firstIterLSH = TRUE;
  int countLSH=0;
  int firstTime=0;
  while((countLSH<2) || !centroidsConverge(clusters,oldClusters,numOfClusters,d)){
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
    reverseAssignmentLSH(lsh,vectors,clusters,oldClusters,clustersHt,numOfClusters,countLSH,&firstTime);

    firstIterLSH=FALSE;

  }

  for(int i=0;i<numOfClusters;i++){
    fprintf(fptr,"CLUSTER-%d {size: %d",i+1,getNumberOfVectors(clustersHt[i]));
    printVectorInFile(clusters[i],fptr);
    fprintf(fptr,"}\n");
  }

  printf("- COMPUTING SILHOUETTES FOR CLUSTERS\n");
  double * silhouettes = silhouetteLSH_Hypercube(clustersHt,clusters,numOfClusters);
  printf("- FINISHED COMPUTING SILHOUETTES\n");
  for(int i=0;i<numOfClusters;i++){
    printf("CLUSTER-%d { silhouette: %f }\n",i+1,silhouettes[i]);
  }

  for(int i=0;i<numOfClusters;i++){
    if(oldClusters[i]!=NULL)
      deleteVector(oldClusters[i]);
    if(clusters[i]!=NULL)
      deleteVector(clusters[i]);
    htDelete(clustersHt[i],0);
  }

  free(silhouettes);
  free(props);
  free(vectors);
  free(oldClusters);
  free(clustersHt);
  free(clusters);
  destroyLSH(lsh);
}

void reverseAssignmentHypercube(HyperCube cube,Vector *vectors,Vector *clusters,Vector *oldClusters,HashTable *clustersHt,int numOfClusters,int iteration,int m,int probes,int *firstTime){
  printf("ITERATION WITH LSH %d\n",iteration);
  if(*firstTime) //skip it for the first time
    for(int i=0;i<numOfClusters;i++){

      Vector newCenter = htMeanOfCluster(clustersHt[i],d);
      if(newCenter==NULL){
        newCenter=copyVector(oldClusters[i]);
      }

      htDelete(clustersHt[i],0);
      clustersHt[i] = htInitialize(numOfVecs/(4*numOfClusters));

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
      printf("******* ---- ASSINGED ITEMS = %d\n",assignCounter);
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
  *firstTime=1;
}


void clusteringHypercube(List vecList,int numOfClusters,int m,int probes,FILE* fptr){
  Vector *vectors;
  Vector *clusters;
  Vector *oldClusters = NULL;
  double *props;
  HashTable *clustersHt=malloc(numOfClusters*sizeof(HashTable *));
  for(int i=0;i<numOfClusters;i++){
    clustersHt[i]= htInitialize(numOfVecs/(4*numOfClusters)); // TODO: CHANGE SIZE
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
  // for(int i=0;i<numOfClusters;i++){
  //   printf("- CLUSTER :%d\n",i);
  //   printVector(clusters[i]);
  // }

  hashTableSize=numOfVecs/8;
  HyperCube cube = initializeHyperCube();
  for(int i=0;i<numOfVecs;i++){
    initializeClusterInfo(vectors[i]);
    insertToHyperCube(cube,vectors[i]);
  }

  int firstIterLSH = TRUE;
  int countLSH=0;
  int firstTime=0;
  while((countLSH<2) || !centroidsConverge(clusters,oldClusters,numOfClusters,d)){
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
    reverseAssignmentHypercube(cube,vectors,clusters,oldClusters,clustersHt,numOfClusters,countLSH,m,probes,&firstTime);

    firstIterLSH=FALSE;
  }

  for(int i=0;i<numOfClusters;i++){
    fprintf(fptr,"CLUSTER-%d {size: %d",i+1,getNumberOfVectors(clustersHt[i]));
    printVectorInFile(clusters[i],fptr);
    fprintf(fptr,"}\n");
  }

  printf("- COMPUTING SILHOUETTES FOR CLUSTERS\n");
  double * silhouettes = silhouetteLSH_Hypercube(clustersHt,clusters,numOfClusters);
  printf("- FINISHED COMPUTING SILHOUETTES\n");
  for(int i=0;i<numOfClusters;i++){
    printf("CLUSTER-%d { silhouette: %f }\n",i+1,silhouettes[i]);
  }

  for(int i=0;i<numOfClusters;i++){
    if(oldClusters[i]!=NULL)
      deleteVector(oldClusters[i]);
    if(clusters[i]!=NULL)
      deleteVector(clusters[i]);
    htDelete(clustersHt[i],0);
  }

  free(silhouettes);
  free(props);
  free(vectors);
  free(oldClusters);
  free(clustersHt);
  free(clusters);
  deleteHyperCube(cube);
}

void clustering(List vecList,FILE* fptr,char* method,int numOfClusters,int l,int mHyper,int probes){
  if(strcmp(method,"Classic")==0){
    fprintf(fptr,"Algorithm: Lloyds\n");
    clusteringLloyds(vecList,numOfClusters,fptr);
  }
  else if(strcmp(method,"LSH")==0){
    fprintf(fptr,"Algorithm: Range Search LSH\n");
    clusteringLSH(vecList,numOfClusters,l,fptr);
  }
  else if(strcmp(method,"HyperCube")==0){
    fprintf(fptr,"Algorithm: Range Search Hypercube\n");
    clusteringHypercube(vecList,numOfClusters,mHyper,probes,fptr);
  }
  else{
    printf("%s\n",method );
    printf("INVALID METHOD NAME!\n");
    exit(EXIT_FAILURE);
  }

}
