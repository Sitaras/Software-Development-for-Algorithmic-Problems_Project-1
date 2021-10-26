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


void printOptions(){
  printf("_________________Options____________________\n\n");
	printf("1. /repeat <new_input_file>\n\n");
	printf("2. /exit\n\n");
	printf("_____________________________________\n\n");
}

int main(int argc, char *argv[])  {

  int option;
  char str[200];
  char inputFile[100];
  int inputflag=0;
  char queryFile[100];
  int queryflag=0;
  char outputFile[100];
  strcpy(outputFile,"outputTestingLSH");
  int outputflag=0;
  int l=6;
  int n=1;
  int r=10000;
  k=4;

  while((option = getopt(argc, argv, "i:q:k:L:o:N:R:")) != -1){
     switch(option){
        case 'i':
        inputflag++;
        strcpy(inputFile,optarg);
        printf("Given input File : %s\n", inputFile);
        break;

        case 'q':
        queryflag++;
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
        outputflag++;
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

  // if(!inputflag){
  //   printf(">Input file name: ");
  //   fflush(stdin); // clear stdin buffer
  //   if (fgets(str, sizeof(char)*200, stdin) == NULL) { // read a command
  //     perror("Error reading string with fgets\n");
  //     exit(1);
  //   }
  //   strcpy(inputFile,str);
  //   printf("Given input File : %s\n", inputFile);
  // }
  // if(!queryflag){
  //   printf(">Query file name: ");
  //   fflush(stdin); // clear stdin buffer
  //   if (fgets(str, sizeof(char)*200, stdin) == NULL) { // read a command
  //     perror("Error reading string with fgets\n");
  //     exit(1);
  //   }
  //   strcpy(queryFile,str);
  //   printf("Given query File : %s\n", queryFile);
  // }
  // if(!outputflag){
  //   printf(">Output file name: ");
  //   fflush(stdin); // clear stdin buffer
  //   if (fgets(str, sizeof(char)*200, stdin) == NULL) { // read a command
  //     perror("Error reading string with fgets\n");
  //     exit(1);
  //   }
  //   strcpy(outputFile,str);
  //   printf("Given output File : %s\n", outputFile);
  // }

  srand(time(NULL));
  hashTableSize = 50;
  k = 6;
  w = 8;
  d = findDim("input_small_id");
  // d = findDim("testing.txt");
  printf("DIMENSION = %d\n",d);

  LSH temp = initializeLSH(l);
  readFile("input_small_id",temp);
  // printLSH(temp);
  readQueryFile("query_small_id",outputFile,temp);

  // double vec[128] = {1.0 , 1.0};
  // for (int i = 3; i < d; i++) {
  //   vec[d]=0;
  // }
  // double vec2[128] = {0,16,35,5,32,31,14,10,11,78,55,10,45,83,11,6,14,57,102,75,20,8,3,5,67,17,19,26,5,0,1,22,60,26,7,1,18,22,84,53,85,119,119,4,24,18,7,7,1,81,106,102,72,30,6,0,9,1,9,119,72,1,4,33,119,29,6,1,0,1,14,52,119,30,3,0,0,55,92,111,2,5,4,9,22,89,96,14,1,0,1,82,59,16,20,5,25,14,11,4,0,0,1,26,47,23,4,0,0,4,38,83,30,14,9,4,9,17,23,41,0,0,2,8,19,25,23,1};
  // Vector vecTmp=initVector(vec2);
  // //
  // FILE* fPtr;
  // fPtr = fopen(outputFile, "w");
  // if(fPtr == NULL){
  //   /* File not created hence exit */
  //   printf("Unable to create file.\n");
  //   exit(EXIT_FAILURE);
  // }
  // fprintf(fPtr, "Query:\n");
  // printf("================================================\n");
  // nearestNeigbor(temp,vecTmp,fPtr);
  // printf("================================================\n");
  // kNearestNeigbors(temp, vecTmp, 3,fPtr);
  // printf("================================================\n");
  // // printLSH(temp);
  // radiusNeigbor(temp,vecTmp,200.0,fPtr);


  // printOptions(); // just printing the commands options for the user


  // char command[200];
  // while(1){
  //
  //   printf("\n");
  //   printf(">Enter a command: ");
  //   fflush(stdin); // clear stdin buffer
  //   if (fgets(str, sizeof(char)*200, stdin) == NULL) { // read a command
  //     perror("Error reading string with fgets\n");
  //     exit(1);
  //   }
  //   else if(strstr(str, "/repeat") != NULL) {
  //     sscanf(str,"%s %s\n",command,inputFile);
  //     printf("FILE: %s\n",inputFile);
  //     continue;
  //   }
  //   else if(strcmp(str,"/exit\n")==0){
  //     break;
  //   }
  //   else{
  //     printf("\n\n  --- Wrong command ! Please, try again. ---  \n\n");
  //     printOptions(); // just printing the commands options for the user
  //     continue;
  //   }
  //
  // }

  // deleteVector(vecTmp);

  destroyLSH(temp);
  // fclose(fPtr);


  return 0;
}
