#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Vector/vector.h"
#include "./hashTable/hashTable.h"
#include "Hypercube/hypercube.h"
#include "./parsing/parsingCube.h"
int d;
int k;
int m;
int probes;
int w;

int main(int argc, char const *argv[]) {
  srand(time(NULL));
  k = 4;
  w = 6;
  d = findDim("testing2.txt");
  printf("DIMENSION = %d\n",d);

  HyperCube hc = initializeHyperCube();
  readFile("testing2.txt",hc);

  printHyperCube(hc);

  deleteHyperCube(hc);
  return 0;
}
