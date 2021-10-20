#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Vector/vector.h"
#include "./hashTable/hashTable.h"
#include "./parsing/parsing.h"
#include "LSH/lsh.h"

int d;

int w;
int k;
int hashTableSize;

int main(int argc, char const *argv[]) {

// test
  // srand(time(NULL));
  int l = 3;
  LSH temp = initializeLSH(l);
  readFile("testing.txt");
  destroyLSH(temp);



  return 0;
}
