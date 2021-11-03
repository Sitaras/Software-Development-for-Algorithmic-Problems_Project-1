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

int binarySearchKmeans( double key, double data[], const int len )
{
    int low  = 0;
    int high = len-1;

    while( low <= high)
    {
        // To convert to Javascript:
        // var mid = low + ((high - low) / 2) | 0;
        int mid = low + ((high - low) / 2);

        /**/ if (data[mid] < key) low  = mid + 1;
        else if (data[mid] > key) high = mid - 1;
        else return                      mid + 1;
    }

    if( high < 0 )
        return 0;   // key < data[0]
    else
    if( low > (len-1))
        return len; // key >= data[len-1]
    else
        return (low < high)
            ? low  + 1
            : high + 1;
}



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
    // for(int r = 1; r<numOfVecs-t; r++){
    //   if(p[r-1]<x && x<=p[r]){
    //     selectedCluster[t] = index[r];
    //     clusters[t++] = copyVector(vecs[index[r]]);
    //     break;
    //   }
    // }
    int bin_index = binarySearchKmeans(x,p,numOfVecs-t);
    selectedCluster[t] = index[bin_index];
    clusters[t++] = copyVector(vecs[index[bin_index]]);
  }
}
