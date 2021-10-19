// #include "hashTableList.h"



typedef struct node{
  int* coords;
}vec;
typedef struct vec *Vector;

struct listNode {
  vector *v;
  listNode* next;
};
typedef struct listNode *List;


// hash table's linked list

List allocateListNode(Vector v,int *dimensions){
  List node=malloc(sizeof(List));
  initVector()
  return node;
}

void initVector(Vector v,int *dimensions){
  // v=malloc(sizeof());

}

void deleteVector(){

}


List listInsert(List list,vector v){
  if (list==NULL){
    // list is empty
    List node=allocateListNode(v);
    list=node;
    return list;
  }
  //////////////////////////////////////////////////
  // Just push
  // List* node=allocateListNode(v);
  // node->next=list;
  // list=node;
  //////////////////////////////////////////////////
  // insert a new node at the end of the list
  List temp = list;
  while (temp->next!=NULL){
        temp = temp->next;
  }

  List node=allocateListNode(v);
  list->next=node;
  //////////////////////////////////////////////////



  return list;
}


List listSearchId(List list, int id){
  List current=list;
  while (current!=NULL){
      if (current->n.id==id)
          return current;
      current=current->next;
  }
  return NULL;
}


void listPrint(List list){
    if(list==NULL) return;
    List temp=list;
    while(temp!=NULL){
        printf(" --Id:%d-Bit:%d-- ",temp->n.id,temp->n.bit);
        temp=temp->next;
    }
}


List listDelete(List list){
    // delete whole list
    if(list==NULL) return NULL;

    List current = list;
    List next;

    while(current!=NULL){
        next=current->next;
        deleteVector(current->v);
        free(current);
        current=next;
    }
    list=NULL;
    return list;
}
