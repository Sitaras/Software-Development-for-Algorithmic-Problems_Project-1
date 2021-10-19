#include "hashTable.h"


typedef struct hashtable_node *hashtable_nodePtr;
typedef struct hashtable *hashtablePtr;

struct hashtable_node{
  int key;
  List head;
};

struct hashtable{
  hashtable_node* table;
  int buckets;
  int numberOfVectors;
};



int hashFunction(const hashtable* ht,int id){
  return (id % ht->buckets);
}


void htInitialize(hashtable* ht,int buckets) {
  ht->table=malloc(sizeof(hashtable_node)*buckets);
  ht->buckets=buckets;
  for (int i=0;i<buckets;i++){
    ht->table[i].key=i;
    ht->table[i].head=NULL;
  }
  ht->numberOfNodes=0; // nodes counter
}


node* htSearch(const hashtable* ht, int id){
  int index=hashFunction(ht, id);
  listNode* ln=listSearchId(ht->table[index].head,id);
  if (ln != NULL){
    // found
    return &(ln->n);
  }
  else {
    // not found
    return NULL;
  }
}


int htUniqueInsert(hashtable* ht, vector v){
  int index=hashFunction(ht,v.id);
  ht->table[index].head=listInsert(ht->table[index].head,v);
  ht->numberOfNodes++;
  return 1;
}


void htPrint(const hashtable* ht){
  for (int i=0;i<ht->buckets;i++){
    printf("\n>>Bucket %d: ",i+1);
    listPrint(ht->table[i].head);
  }
  printf("\n\n");
}


void htDelete(hashtable* ht){
  // delete whole hash table
  for (int i=0;i<ht->buckets;i++){
    ht->table[i].head=listDelete(ht->table[i].head);
  }
  free(ht->table);
}
