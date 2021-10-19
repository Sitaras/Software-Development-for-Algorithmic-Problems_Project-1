#include "hashTableList.h"

// hash table's linked list

listNode* allocateListNode(vector v){
  listNode* node=malloc(sizeof(listNode));
  node->n.id=v.id;
  node->n.bit=v.bit;
  node->next=NULL;
  return node;
}


listNode* listInsert(listNode* list,vector v){
  if (list==NULL){
    // list is empty
    listNode* node=allocateListNode(v);
    list=node;
    return list;
  }
  //////////////////////////////////////////////////
  // Just push
  // listNode* node=allocateListNode(v);
  // node->next=list;
  // list=node;
  //////////////////////////////////////////////////
  // insert a new node at the end of the list
  listNode* temp = list;
  while (temp->next!=NULL){
        temp = temp->next;
  }

  listNode* node=allocateListNode(v);
  list->next=node;
  //////////////////////////////////////////////////



  return list;
}


listNode* listSearchId(listNode *list, int id){
  listNode* current=list;
  while (current!=NULL){
      if (current->n.id==id)
          return current;
      current=current->next;
  }
  return NULL;
}


void listPrint(listNode *list){
    if(list==NULL) return;
    listNode *temp=list;
    while(temp!=NULL){
        printf(" --Id:%d-Bit:%d-- ",temp->n.id,temp->n.bit);
        temp=temp->next;
    }
}


listNode* listDelete(listNode *list){
    // delete whole list
    if(list==NULL) return NULL;

    listNode *current = list;
    listNode *next;

    while(current!=NULL){
        next=current->next;
        free(current);
        current=next;
    }
    list=NULL;
    return list;
}
