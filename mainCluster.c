#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "Vector/vector.h"
#include "./hashTable/hashTable.h"
#include "./hashTable/hashTableList/hashTableList.h"
#include "Hypercube/hypercube.h"
#include "./parsing/parsingCluster.h"
#include "./Clustering/clustering.h"
int d;
int k;
int m;
int probes;
int w;
int numOfVecs;
int hashTableSize;

void printOptions(){
  printf("_________________Options____________________\n\n");
	printf("1. /repeat <new_input_file>\n\n");
	printf("2. /exit\n\n");
	printf("_____________________________________\n\n");
}



int main(int argc, char *argv[]) {
  srand(time(NULL));
  int option;
  char str[200];
  char inputFile[100];
  int inputflag=0;
  char confFile[100];
  int confflag=0;
  char outputFile[100];
  int outputflag=0;
  int m=10;
  int n=1;
  int checkflag=0;
  k=14;

  while((option = getopt(argc, argv, "i:c:o:M:m:")) != -1){
     switch(option){
        case 'i':
        inputflag++;
        strcpy(inputFile,optarg);
        printf("Given input File : %s\n", inputFile);
        break;

        case 'c':
        confflag++;
        strcpy(confFile,optarg);
        printf("Given configuration File : %s\n", confFile);
        break;

        case 'M':
        m=atoi(optarg);
        printf("L : %d\n", m);
        break;

        case 'o':
        outputflag++;
        strcpy(outputFile,optarg);
        printf("Given output File : %s\n", outputFile);
        break;

        case ':':
         printf("option needs a value\n");
         break;
        default: /* '?' */
          fprintf(stderr, "Usage: %s –i <input file> –c <configuration file> -o <output file> -complete <optional> -m <method: Classic OR LSH or Hypercube>\n",argv[0]);
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
  // if(!confflag){
  //   printf(">Conf file name: ");
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

  k = 4;
  w = 6;
  d = findDim("query_small_id");
  printf("DIMENSION = %d\n",d);

  List list = initializeList();
  readConfFile("cluster.conf");
  readFile("query_small_id",&list,&numOfVecs);

  clustering(list,5);


  // printHyperCube(hc);
  // listPrint(list);

  double vec[6] = {21,  3,  3,  18,  25,  11};
  Vector vecTmp=initVector(vec);



  // printOptions(); // just printing the commands options for the user
  //
  //
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

  listDelete(list,1);
  deleteVector(vecTmp);
  return 0;
}
