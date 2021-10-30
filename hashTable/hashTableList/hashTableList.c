#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "../../Vector/vector.h"
#include "../hashTable.h"


#define SQUARE(x) ((x)*(x))


double distance_metric(Vector v1,Vector v2,int d){
  double sum = 0.0;
  double *coords1 = getCoords(v1);
  double *coords2 = getCoords(v2);
  for(int i=0;i<d;i++){
    sum += SQUARE(coords1[i]-coords2[i]);
  }
  return sqrt(sum);
}


typedef struct listNode *List;
struct listNode {
  Vector v;
  int vector_ID;
  List next;
};


List initializeList(){
  return NULL;
}


// hash table's linked list

List allocateListNode(Vector v,int id){
  List node=malloc(sizeof(struct listNode));
  node->v=v;
  node->vector_ID=id;
  node->next=NULL;
  return node;
}



List listInsert(List list,Vector v,int id){
  if (list==NULL){
    // list is empty
    List node=allocateListNode(v,id);
    return node;
  }
  List newnode = allocateListNode(v,id);
  newnode->next = list;

  return newnode;
}

List listUniqueInsert(List list,Vector v,int id){
  if (list==NULL){
    // list is empty
    List node=allocateListNode(v,id);
    return node;
  }
  List origin = list;
  while(list!=NULL){
    if(compareVectors(v, list->v)){
      return origin;
    }
    list=list->next;
  }

  List newnode = allocateListNode(v,id);
  newnode->next = origin;

  return newnode;
}

List listDeleteItem(List list,Vector v,int id){
  if (list==NULL){
    // list is empty
    return list;
  }
  List origin = list;
  List prev = NULL;
  while(list!=NULL){
    if(compareVectors(v, list->v)){
      if(prev==NULL){
        origin = list->next;
      }else{
        prev->next = list->next;
      }
      free(list);
      return origin;
    }
    prev = list;
    list=list->next;
  }

  return origin;
}


// List listSearchId(List list, int id){
//   List current=list;
//   while (current!=NULL){
//       if (current->n.id==id)
//           return current;
//       current=current->next;
//   }
//   return NULL;
// }
//
//
void listPrint(List list){
    if(list==NULL){ printf("List Empty!\n");  return;}
    List temp=list;
    while(temp!=NULL){
        printVector(temp->v);
        temp=temp->next;
    }
}
void listRangePrint(List list,Vector q,int d,int *counter,FILE *fptr){
    if(list==NULL){ return;}
    List temp=list;
    while(temp!=NULL){
        // printf("%d: ",(*counter));
        fprintf(fptr,"%d: ",(*counter)++);
        fprintf(fptr,"Found vector: ");
        // printVectorId(temp->v);
        printVectorIdInFile(temp->v,fptr);
        // printf("inside range with distance: %f\n",distance_metric(temp->v,q,d));
        // fprintf(fptr,"inside range with distance: %f\n",distance_metric(temp->v,q,d));
        temp=temp->next;
    }
}


Vector *transformListToArray(List list,int size){
  Vector *array = malloc(size*sizeof(Vector));
  List temp = list;
  int index=0;
  while(temp!=NULL){
      if(index>=size){
        printf("!! OUT OF RANGE (transformListToArray)\n");
        break;
      }
      array[index++] = temp->v;
      temp=temp->next;
  }
  return array;
}


List listDelete(List list,int freeVectors){
    // delete whole list
    if(list==NULL) return NULL;

    List current = list;
    List next;

    while(current!=NULL){
        next=current->next;
        if(freeVectors){
          deleteVector(current->v);
        }
        free(current);
        current=next;
    }
    list=NULL;
    return list;
}

void listFindNearestNeighbor(List list,Vector q,Vector *nearest,double *nearestDist,int d,int id){
  if(list==NULL){ return;}
  List temp=list;
  while(temp!=NULL){
    if(id==(temp->vector_ID)){
      double dist = distance_metric(temp->v,q,d);
      if(dist<(*nearestDist) || (*nearestDist)<0){
        (*nearestDist) = dist;
        (*nearest) = temp->v;
      }
    }
    temp=temp->next;
  }
}

void listFindNearestNeighborCube(List list,Vector q,Vector *nearest,double *nearestDist,int d,int *numOfSearched,int maxToSearch){
  if(list==NULL){ return;}
  List temp=list;
  while(temp!=NULL){
    if((*numOfSearched)>=maxToSearch){
      return;
    }
    double dist = distance_metric(temp->v,q,d);
    if(dist<(*nearestDist) || (*nearestDist)<0){
      (*nearestDist) = dist;
      (*nearest) = temp->v;
    }
    (*numOfSearched)+=1;
    temp=temp->next;
  }
}

void swapDoubles(double *xp, double *yp)
{
  double temp = *xp;
  *xp = *yp;
  *yp = temp;
}
void swapVectors(Vector *xp, Vector *yp)
{
  Vector temp = *xp;
  *xp = *yp;
  *yp = temp;
}

// void bubbleSort(double arr[], Vector *nearest, int n)
// {
//   for (int i = 0; i < n - 1; i++){
//     for (int j = 0; j < n - i - 1; j++){
//       if (arr[j] < arr[j + 1]){
//         swapDoubles(&arr[j], &arr[j + 1]);
//         swapVectors(&nearest[j], &nearest[j + 1]);
//       }
//     }
//   }
// }


// function to find the partition position
int partition(double arr[], Vector *nearest, int low, int high){
  double pivot = arr[high];
  int i = (low - 1);
  for (int j = low; j < high; j++) {
    if (arr[j] >= pivot) {

      i++;

      swapDoubles(&arr[i], &arr[j]);
      swapVectors(&nearest[i], &nearest[j]);
    }
  }

  swapDoubles(&arr[i + 1], &arr[high]);
  swapVectors(&nearest[i + 1], &nearest[high]);

  return (i + 1);
}

void quickSort(double arr[], Vector *nearest, int low, int high){
  if (low < high) {

    int pi = partition(arr, nearest, low, high);

    quickSort(arr,nearest, low, pi - 1);

    quickSort(arr, nearest, pi + 1, high);
  }
}

int binarySearch(double arr[], int l, int r, double x){
  if (r >= l){
    int mid = l + (r - l) / 2;
    if (arr[mid] == x)
      return mid;

    if (arr[mid] < x)
      return binarySearch(arr, l, mid - 1, x);

    return binarySearch(arr, mid + 1, r, x);
  }

  return -1;
}

void listFindKNearestNeighbors(List list,Vector q,Vector *nearest,double *nearestDist,int d,int k,int id){
  if(list==NULL){ return;}
  List temp=list;
  int filled=0;
  while(temp!=NULL){
      if(id!=(temp->vector_ID)){
        temp = temp->next;
        continue;
      }
      int flag = 1;
      int eq = 0;
      int added=0;
      double dist = distance_metric(temp->v,q,d);

      int index=binarySearch(nearestDist,0,k-1,dist);
      if(index!=-1 && nearest[index] != NULL && compareVectors(nearest[index], temp->v)){
        eq = 1;
      }
      if (eq){
        temp = temp->next;
        continue;
      }
      if(!filled)
        for (int i = 0; i < k; i++){
          if(nearestDist[i]<0){
            flag=0;
            added=1;
            nearestDist[i]=dist;
            nearest[i]=temp->v;
            break;
          }
        }

      if (flag){
        filled=1;
          if(dist<nearestDist[0]){
            added = 1;
            nearestDist[0] = dist;
            nearest[0] = temp->v;
          }
      }
      if(added)
        quickSort(nearestDist, nearest,0, k-1);
      temp = temp->next;
  }
}

void listFindKNearestNeighborsCube(List list,Vector q,Vector *nearest,double *nearestDist,int d,int k,int *numOfSearched,int maxToSearch){
  if(list==NULL){ return;}
  List temp=list;
  int filled=0;
  while(temp!=NULL){
      if((*numOfSearched)>=maxToSearch){
        return;
      }
      int flag = 1;
      int eq = 0;
      int added=0;
      double dist = distance_metric(temp->v,q,d);
      (*numOfSearched) += 1;

      int index=binarySearch(nearestDist,0,k-1,dist);
      if(index!=-1 && nearest[index] != NULL && compareVectors(nearest[index], temp->v)){
        eq = 1;
      }
      if (eq){
        temp = temp->next;
        continue;
      }
      if(!filled)
        for (int i = 0; i < k; i++){
          if(nearestDist[i]<0){
            flag=0;
            added=1;
            nearestDist[i]=dist;
            nearest[i]=temp->v;
            break;
          }
        }

      if (flag){
        filled=1;
          if(dist<nearestDist[0]){
            added = 1;
            nearestDist[0] = dist;
            nearest[0] = temp->v;
          }
      }
      if(added)
        quickSort(nearestDist, nearest,0, k-1);
      temp = temp->next;
  }
}






void listFindNeighborsInRadius(List list,HashTable storeNeighbors,Vector q,int d,int id,int radius){
  if(list==NULL){ return;}
  List temp=list;
  while(temp!=NULL){
    if(id==(temp->vector_ID)){
      double dist = distance_metric(temp->v,q,d);
      if(dist<=radius){
        htRangeInsert(storeNeighbors,temp->v,temp->vector_ID,d);
      }
    }
    temp=temp->next;
  }
}

void listFindNeighborsInRadiusClustering(List list,int centroidIndex,List* confList,HashTable storeNeighbors,Vector q,int d,int id,int radius,int *assignCounter,int iteration){
  if(list==NULL){ return;}
  List temp=list;
  while(temp!=NULL){
    if(id==(temp->vector_ID)){
      double dist = distance_metric(temp->v,q,d);
      if(dist<=radius){

        if(assignedToCluster(temp->v) && (getAssignedIteration(temp->v)==iteration)){
          int assignedCluster = getAssignedCluster(temp->v);
          if(assignedCluster==centroidIndex || (((int)getAssignedAtRadius(temp->v))!=radius)){
          // if(assignedCluster==centroidIndex){
            temp=temp->next;
            continue;
          }else{
            *confList=listUniqueInsert(*confList,temp->v,-1);
            // printf("SURE NOT NULL\n");
            temp=temp->next;
            continue;
          }
        }else{
          htRangeInsert(storeNeighbors,temp->v,temp->vector_ID,d);
          setAssignedCluster(temp->v,centroidIndex);
          setAssignedIteration(temp->v,iteration);
          setAssignedAtRadius(temp->v,radius);
          (*assignCounter)++;
        }
        // move it at the end of the list
      }
    }
    temp=temp->next;
  }
}

void listFindNeighborsInRadiusClusteringCube(List list,int centroidIndex,List* confList,HashTable storeNeighbors,Vector q,int d,int radius,int *numOfSearched,int maxToSearch,int *assignCounter,int iteration){
  if(list==NULL){ return;}
  List temp=list;
  while(temp!=NULL){
      if((*numOfSearched)>=maxToSearch){
        return;
      }
      (*numOfSearched)++;
      double dist = distance_metric(temp->v,q,d);
      if(dist<=radius){
        if(assignedToCluster(temp->v) && (getAssignedIteration(temp->v)==iteration)){
          int assignedCluster = getAssignedCluster(temp->v);
          if(assignedCluster==centroidIndex || (((int)getAssignedAtRadius(temp->v))!=radius)){
          // if(assignedCluster==centroidIndex){
            temp=temp->next;
            continue;
          }else{
            *confList=listUniqueInsert(*confList,temp->v,-1);
            // printf("SURE NOT NULL\n");
            temp=temp->next;
            continue;
          }
        }else{
          htRangeInsert(storeNeighbors,temp->v,temp->vector_ID,d);
          setAssignedCluster(temp->v,centroidIndex);
          setAssignedIteration(temp->v,iteration);
          setAssignedAtRadius(temp->v,radius);
          (*assignCounter)++;
        }
        // move it at the end of the list
      }
    temp=temp->next;
  }
}

void listSolveRangeConflicts(List conflictList,HashTable *clustersHt,Vector *clusters,int numOfClusters,int d){
  if(conflictList==NULL){ return;}
  List temp=conflictList;
  while(temp!=NULL){
    htRangeDelete(clustersHt[getAssignedCluster(temp->v)],temp->v,temp->vector_ID,d);
    double minDist = DBL_MAX;
    int closestCentroid = -1;
    for(int i=0;i<numOfClusters;i++){
      double dist = distance_metric(temp->v,clusters[i],d);
      if(dist<minDist){
        minDist = dist;
        closestCentroid = i;
      }
    }
    htRangeInsert(clustersHt[closestCentroid],temp->v,temp->vector_ID,d);
    setAssignedCluster(temp->v,closestCentroid);

    temp=temp->next;
  }

}

void listFindNeighborsInRadiusCube(List list,HashTable storeNeighbors,Vector q,int d,int radius,int *numOfSearched,int maxToSearch){
  if(list==NULL){ return;}
  List temp=list;
  while(temp!=NULL){
    if((*numOfSearched)>=maxToSearch){
      return;
    }
    double dist = distance_metric(temp->v,q,d);
    (*numOfSearched) += 1;
    if(dist<=radius){
      htRangeInsert(storeNeighbors,temp->v,temp->vector_ID,d);
    }
    temp=temp->next;
  }
}


Vector listMeanOfCluster(List list,int d){
  if(list==NULL) return NULL;
  List temp = list;
  int count=0;
  double *sumDims=calloc(d,sizeof(double));
  while(temp!=NULL){
    for(int i=0;i<d;i++){
      sumDims[i]+=getCoords(temp->v)[i];
    }
    count++;
    temp=temp->next;
  }
  for(int i=0;i<d;i++){
    sumDims[i]/=(double)count;
  }

  Vector newCentroid  = initVector(sumDims,"tempCentroid");
  free(sumDims);

  return newCentroid;
}

double *listSumOfVectors(List list,int d,int *count){
  if(list==NULL) return NULL;
  List temp = list;
  double *sumDims=calloc(d,sizeof(double));
  while(temp!=NULL){
    for(int i=0;i<d;i++){
      sumDims[i]+=getCoords(temp->v)[i];
    }
    (*count)++;
    temp=temp->next;
  }
  return sumDims;
}
