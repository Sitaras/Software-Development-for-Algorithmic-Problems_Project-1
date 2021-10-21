#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "Vector/vector.h"
#include "./hashTable/hashTable.h"
#include "Hypercube/hypercube.h"
#include "./parsing/parsingCube.h"
int d;
int k;
int m;
int probes;
int w;

int main(int argc, char *argv[]) {
  srand(time(NULL));
  int option;
  char inputFile[100];
  char queryFile[100];
  char outputFile[100];
  int m=10;
  int n=1;
  int r=10000;
  int probes=2;
  int checkflag=0;
  k=14;

  while((option = getopt(argc, argv, "i:q:k:M:p:o:N:R:")) != -1){
     switch(option){
        case 'i':
        checkflag++;
        strcpy(inputFile,optarg);
        printf("Given input File : %s\n", inputFile);
        break;

        case 'q':
        checkflag++;
        strcpy(queryFile,optarg);
        printf("Given query File : %s\n", queryFile);
        break;

        case 'k':
        k=atoi(optarg);
        printf("k : %d\n", k);
        break;

        case 'M':
        m=atoi(optarg);
        printf("L : %d\n", m);
        break;

        case 'p':
        probes=atoi(optarg);
        printf("probes : %d\n", probes);
        break;

        case 'o':
        checkflag++;
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
          fprintf(stderr, "Usage: %s –i <input file> –q <query file> –k <int> -M <int> -probes <int> -ο <output file> -Ν <number of nearest> -R <radius>\n",argv[0]);
          exit(EXIT_FAILURE);
     }
  }


  if(checkflag!=3){
    // fprintf(stderr, "Usage: %s –i <input file> –q <query file> –k <int> -M <int> -probes <int> -ο <output file> -Ν <number of nearest> -R <radius>\n",argv[0]);
    // exit(EXIT_FAILURE);
  }

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
