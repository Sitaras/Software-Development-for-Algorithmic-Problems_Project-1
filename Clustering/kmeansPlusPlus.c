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
#include "../LSH/helperFunctions.h"
#include "./clusterHelpingFuns.h"


#define SQUARE(x) ((x)*(x))

extern int numOfVecs;


void kmeansplusplus(Vector* vecs,int numOfClusters,Vector* clusters,double *props){
  int t=1;
  int selectedCluster[numOfClusters];
  for(int i=0;i<numOfClusters;i++){
    selectedCluster[i] = -1;
  }
  int selectFirstCluster = rand() % numOfVecs;
  selectedCluster[0] = selectFirstCluster;
  clusters[0] = copyVector(vecs[selectFirstCluster]);



  while(t<numOfClusters){
    double maxDi = -1.0;
    for(int i = 0; i<numOfVecs;i++){
      if(existsInArray(selectedCluster,i,numOfClusters)){
        continue;
      }
      double minDist = DBL_MAX;
      props[i]=0;
      minDistToCentroids(vecs[i],vecs,clusters,numOfClusters,&minDist);
      props[i] = minDist;
      if(minDist>maxDi){
        maxDi=minDist;
      }
    }
    double p[numOfVecs-t];
    int index[numOfVecs-t];
    p[0]=0.0;
    index[0]=0;
    double sum = 0;
    int count=1;
    for(int r = 1; r<numOfVecs; r++){
      if(existsInArray(selectedCluster,r,numOfClusters)){
        continue;
      }
      sum += SQUARE(props[r]/maxDi);
      index[count] = r;
      p[count++] = sum;
    }

    double x=uniform_distribution(0,p[numOfVecs-t-1]);

    for(int r = 1; r<numOfVecs; r++){
      if(existsInArray(selectedCluster,r,numOfClusters)){
        continue;
      }
      if(p[r-1]<x && x<=p[r]){
        selectedCluster[t] = index[r];
        clusters[t++] = copyVector(vecs[index[r]]);
        break;
      }
    }
  }
}
