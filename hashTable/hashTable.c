#include <stdio.h>
#include <stdlib.h>
#include "../Vector/vector.h"
#include "./hashTableList/hashTableList.h"


struct hashtable_node{
  int key;
  List head;
};
typedef struct hashtable_node *hashtable_nodePtr;


struct hashtable_head{
  hashtable_nodePtr table;
  int buckets;
  int numberOfVectors;
};
typedef struct hashtable_head *HashTable;



// int hashFunction(const HashTable ht,int id){
//   return (id % ht->buckets);
// }


HashTable htInitialize(int buckets) {
  HashTable ht=malloc(sizeof(struct hashtable_head));
  ht->table=malloc(sizeof(struct hashtable_node)*buckets);
  ht->buckets=buckets;
  for (int i=0;i<buckets;i++){
    ht->table[i].key=i;
    ht->table[i].head=initializeList();
  }
  ht->numberOfVectors=0; // nodes counter
  return ht;
}


// hashtable_nodePtr htSearch(const HashTable ht, int id){
//   int index=hashFunction(ht, id);
//   listNode* ln=listSearchId(ht->table[index].head,id);
//   if (ln != NULL){
//     // found
//     return &(ln->n);
//   }
//   else {
//     // not found
//     return NULL;
//   }
// }


int htInsert(HashTable ht, Vector v,int index){
  // int index=hashFunction(ht,1);
  ht->table[index].head=listInsert(ht->table[index].head,v);
  ht->numberOfVectors++;
  return 1;
}


void htPrint(const HashTable ht){
  for (int i=0;i<ht->buckets;i++){
    printf("\n>>Bucket %d: ",i+1);
    listPrint(ht->table[i].head);
  }
  printf("\n\n");
}


HashTable htDelete(HashTable ht){
  // delete whole hash table
  for (int i=0;i<ht->buckets;i++){
    ht->table[i].head=listDelete(ht->table[i].head);
  }
  free(ht->table);
  free(ht);
  return NULL;
}
