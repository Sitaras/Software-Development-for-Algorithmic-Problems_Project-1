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
#include "./clusterHelpingFuns.h"

#define SQUARE(x) ((x)*(x))

extern int numOfVecs;


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
