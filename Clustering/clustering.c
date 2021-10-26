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

#define SQUARE(x) ((x)*(x))

#define TRUE 1
#define FALSE 0
#define CONVERGENCE 50
#define MAX_RECENTER_ITERATIONS 30

extern int numOfVecs;
extern int d;
extern int hashTableSize;

int existsInArray(int *array,int check,int arraySize){
  for(int i=0;i<arraySize;i++){
    if(array[i]==-1){
      return FALSE;
    }
    if(array[i]==check){
      return TRUE;
    }
  }
  return FALSE;
}


void minDistToCentroids(Vector v,Vector* vecs,Vector *clusters,int numOfClusters,double *minDistance){

  for(int i=0;i<numOfClusters;i++){
    if(clusters[i]==NULL){
      break;
    }
    double tempDist = distance_metric(clusters[i],v,d);
    if(tempDist<(*minDistance)){
        (*minDistance) = tempDist;
    }
  }
}

void minDistbetweenCentroids(Vector *centroids,int numOfClusters,double *minDistance){
  // can be impoved
  for(int i=0;i<numOfClusters;i++){
    if(centroids[i]==NULL){
      break;
    }
    for(int j=0;j<numOfClusters;j++){
      if(i!=j){
        if(centroids[j]==NULL){
          break;
        }
        double tempDist = distance_metric(centroids[i],centroids[j],d);
        if(tempDist<(*minDistance)){
          (*minDistance) = tempDist;
        }
      }

    }
  }
}

void kmeansplusplus(Vector* vecs,int numOfClusters,Vector* clusters,double *props){
  int t=1;
  int selectedCluster[numOfClusters];
  for(int i=0;i<numOfClusters;i++){
    selectedCluster[i] = -1;
  }
  int selectFirstCluster = rand()%numOfVecs;
  selectedCluster[0] = selectFirstCluster;
  clusters[0] = copyVector(vecs[selectFirstCluster]);
  double sum = 0.0;
  while(t<numOfClusters){
    for(int i = 0; i<numOfVecs;i++){
      if(existsInArray(selectedCluster,i,numOfClusters)){
        continue;
      }
      double minDist = DBL_MAX;
      minDistToCentroids(vecs[i],vecs,clusters,numOfClusters,&minDist);
      props[i] = minDist;

      sum += SQUARE(minDist);
    }

    double maxProb = DBL_MAX;
    int maxProbIndex = -1;

    for(int i = 0; i<numOfVecs;i++){
      if(existsInArray(selectedCluster,i,numOfClusters)){
        continue;
      }
      props[i] = SQUARE(props[i])/sum;
      if(props[i]<maxProb){
        maxProb=props[i];
        maxProbIndex = i;
      }
    }
    selectedCluster[t] = maxProbIndex;
    clusters[t++] = copyVector(vecs[maxProbIndex]);
  }
}

int centroidsCovnerge(Vector *new,Vector *old,int numOfClusters,int d){
  if(old==NULL) return FALSE;
  for(int i=0;i<numOfClusters;i++){
    printf("%f\n",distance_metric(new[i],old[i],d));
    if(distance_metric(new[i],old[i],d)>CONVERGENCE){
      return FALSE;
    }
  }
  return TRUE;
}

int findClosestCentroid(Vector v,Vector *clusters,int numOfClusters){
  int minDistIndex = -1;
  double minDist = DBL_MAX;
  for(int i=0;i<numOfClusters;i++){
    double tempDist = distance_metric(v,clusters[i],d);
    if(tempDist<minDist){
      minDistIndex = i;
      minDist = tempDist;
    }
  }
  return minDistIndex;
}

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

void clustering(List vecList,int numOfClusters){
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

  free(props);
  free(vectors);
  free(clustersList);
  printf("\n==============================2\n");




  //Reverse approach
  HashTable *clustersHt=malloc(numOfClusters*sizeof(HashTable *));
  for(int i=0;i<numOfClusters;i++){
    clustersHt[i]= htInitialize(50); // TODO: CHANGE SIZE
  }
  double radius=DBL_MAX;
  vectors = transformListToArray(vecList,numOfVecs);
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



  // reverseAssignmentLSH(lsh,clusters,oldClusters,clustersHt,radius,numOfClusters);

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
