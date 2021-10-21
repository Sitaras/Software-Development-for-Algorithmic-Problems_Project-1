#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "../Vector/vector.h"
#include "../hashTable/hashTable.h"
#include "HashMap/hashmap.h"
#include "../LSH/helperFunctions.h"

extern int d;
extern int k;
extern int m;
extern int probes;
extern int w;

typedef struct hfunc{
  double *v;
  double t;
}h_function;

typedef struct hc_n{
  h_function *h_functions;
  HashMap *f_funs;
  HashTable hypercube;
}cubeNode;
typedef cubeNode *HyperCube;

/* Helper Functions */

int binaryToDecimal(int n)
{
    int num = n;
    int dec_value = 0;

    // Initializing base value to 1, i.e 2^0
    int base = 1;

    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;

        dec_value += last_digit * base;

        base = base * 2;
    }

    return dec_value;
}
/* H FUNCTIONS*/

void generateH(h_function *hfun){
  hfun->v=malloc(d*sizeof(double));
  for(int i=0;i<d;i++){
    hfun->v[i] = normalRandom();
  }
  hfun->t=uniform_distribution(0,w);
}

void destroyH(h_function h){
  free(h.v);
}

int computeH(h_function hfun,Vector vector){
  double pv = dot_product(hfun.v,getCoords(vector),d);
  double temp = (double) (pv+hfun.t)/(double)w;
  return floor(temp);
}

/* F Functions */

int flipCoin(){
  return rand()%2;
}

int computeF(HashMap f_fun,Key key){
  int bit = flipCoin();
  Record r = hmSearchOrInsert(f_fun,key,bit);
  return getValue(r);
}


HyperCube initializeHyperCube(){
  HyperCube hc = malloc(sizeof(cubeNode));
  hc->h_functions = malloc(k*sizeof(h_function));
  for(int i=0;i<k;i++){
     generateH(&(hc->h_functions[i]));
  }
  hc->f_funs = malloc(k*sizeof(HashMap));
  for(int i=0;i<k;i++){
    hc->f_funs[i] = hmCreate(100);  // starting size is 100, hashMap is resizable
  }
  hc->hypercube = htInitialize((int)pow(2,k));  // 2^k buckets
  return hc;
}

void insertToHyperCube(HyperCube hc,Vector v){
  int index=0;
  for(int i=0;i<k;i++){
    int h_result = computeH(hc->h_functions[i],v);
    int f_result = computeF(hc->f_funs[i],h_result);
    index *=10;
    index += f_result;
  }
  printf("INDEX = %d\n",index);
  int decimal_index = binaryToDecimal(index); // "hashTable key"
  htInsert(hc->hypercube,v,decimal_index,-1);
}

void printHyperCube(HyperCube hc){
    printf("-------- HyperCube --------\n");
    htPrint(hc->hypercube);
    printf("--------------------------\n");
}

void deleteHyperCube(HyperCube hc){
  for(int i=0;i<k;i++){
    hmDestroy(hc->f_funs[i]);
  }
  free(hc->f_funs);
  htDelete(hc->hypercube,1);
  free(hc);
}
