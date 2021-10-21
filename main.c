#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "Vector/vector.h"
#include "./hashTable/hashTable.h"
#include "LSH/lsh.h"
#include "./parsing/parsingLSH.h"

int d;

int w;
int k;
int hashTableSize;

int main(int argc, char *argv[])  {

  int option;
  char inputFile[100];
  char queryFile[100];
  char outputFile[100];
  int l=5;
  int n=1;
  int r=10000;
  k=4;

  while((option = getopt(argc, argv, "i:q:k:L:o:N:R:")) != -1){
     switch(option){
        case 'i':
        strcpy(inputFile,optarg);
        printf("Given input File : %s\n", inputFile);
        break;

        case 'q':
        strcpy(queryFile,optarg);
        printf("Given query File : %s\n", queryFile);
        break;

        case 'k':
        k=atoi(optarg);
        printf("k : %d\n", k);
        break;

        case 'L':
        l=atoi(optarg);
        printf("L : %d\n", l);
        break;

        case 'o':
        strcpy(outputFile,optarg);
        printf("Given output File : %s\n", outputFile);
        break;

        case 'N':
        n=atoi(optarg);
        printf("number of nearest : %d\n", n);
        break;
        case 'R':
         r=atoi(optarg);
         printf("Radius : %d\n", r);
         break;
        case ':':
         printf("option needs a value\n");
         break;
        default: /* '?' */
          fprintf(stderr, "Usage: %s –i <input file> –q <query file> –k <int> -L <int> -ο <output file> -Ν <number of nearest> -R <radius> name\n",argv[0]);
          exit(EXIT_FAILURE);
     }
  }

  srand(time(NULL));
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
