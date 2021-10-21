#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Vector/vector.h"
#include "./hashTable/hashTable.h"
#include "LSH/lsh.h"
#include "./parsing/parsingLSH.h"

int d;

int w;
int k;
int hashTableSize;

int main(int argc, char const *argv[]) {

// test
  srand(time(NULL));
  int l =10;
  hashTableSize = 5;
  k = 4;
  w = 6;
  d = findDim("testing.txt");
  printf("DIMENSION = %d\n",d);

  LSH temp = initializeLSH(l);
  readFile("testing.txt",temp);
  // printLSH(temp);


  double vec[2] = {1.0 , 1.0};
  Vector vecTmp=initVector(vec);

  nearestNeigbor(temp,vecTmp);
  printf("================================================\n");
  kNearestNeigbors(temp, vecTmp, 3);
  printf("================================================\n");
  radiusNeigbor(temp,vecTmp,20.0);

  deleteVector(vecTmp);

  destroyLSH(temp);



  return 0;
}
