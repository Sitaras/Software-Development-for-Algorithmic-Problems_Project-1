#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../Vector/vector.h"

#define SQUARE(x) ((x)*(x))


typedef struct listNode *List;
struct listNode {
  Vector v;
  List next;
};


List initializeList(){
  return NULL;
}


// hash table's linked list

List allocateListNode(Vector v){
  List node=malloc(sizeof(struct listNode));
  node->v=v;
  node->next=NULL;
  return node;
}



List listInsert(List list,Vector v){
  if (list==NULL){
    // list is empty
    List node=allocateListNode(v);
    return node;
  }
  List newnode = allocateListNode(v);
  newnode->next = list;

  return newnode;
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

double distance_metric(Vector v1,Vector v2,int d){
  double sum = 0.0;
  double *coords1 = getCoords(v1);
  double *coords2 = getCoords(v2);
  for(int i=0;i<d;i++){
    sum += SQUARE(coords1[i]-coords2[i]);
  }
  return sqrt(sum);
}

void listFindNearestNeighbor(List list,Vector q,Vector *nearest,double *nearestDist,int d){
  if(list==NULL){ return;}
  List temp=list;
  while(temp!=NULL){
      double dist = distance_metric(temp->v,q,d);
      if(dist<(*nearestDist) || (*nearestDist)<0){
        (*nearestDist) = dist;
        (*nearest) = temp->v;
      }
      temp=temp->next;
  }
}
