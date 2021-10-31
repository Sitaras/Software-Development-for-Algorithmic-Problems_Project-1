#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../Vector/vector.h"
#include "../hashTable/hashTable.h"
#include "../Hypercube/hypercube.h"
#include "../hashTable/hashTableList/hashTableList.h"

#define OUT    0
#define IN    1
#define FALSE    0
#define TRUE    1

#define MAX_INPUT_LENGTH 2048

extern int d;

// returns number of words in str
int countWords(char *str){
  char * token = strtok(str, " ");
  // printf("NAME = %s\n",token);
  token = strtok(NULL, " ");
   // loop through the string to extract all other tokens
   int counter = 0;
   while( token != NULL ) {
      // printf( " - %s\n", token ); //printing each token
      counter++;
      token = strtok(NULL, " ");
   }

  return counter;
}

int countLines(FILE *fp){
  // count the lines of the given file
  int count=0;
  if(fp==NULL){ // error checking
    perror("Error");
    exit(-1);
  }
  while(!feof(fp)){
    if(fgetc(fp)=='\n'){
      count++;
    }
  }
  return count;
}

int findDim(char* fileName){
  FILE *file = fopen(fileName, "r"); // read mode

  if (file == NULL){
     perror("Error while opening the file.\n");
     exit(-1);
  }

 if (feof(file)){ // empty file, return
   return -1;
 }
  char buffer[MAX_INPUT_LENGTH];
 fflush(stdin);  // clear stdin buffer
 if(fscanf(file,"%[^\n]\n",buffer)<0){ // read a line from the file
   perror("Error while reading the file.\n");
   exit(-1);
 }
 int dims = countWords(buffer)-1;
 fclose(file);
 return dims;
}


void readFile(char* fileName,HyperCube hc,List *inputs){

   FILE *file = fopen(fileName, "r"); // read mode

   if (file == NULL){
      perror("Error while opening the file.\n");
      exit(-1);
   }

  if (feof(file)){ // empty file, return
    return;
  }

  char buffer[MAX_INPUT_LENGTH];

  while(!feof(file)){
    fflush(stdin);  // clear stdin buffer
    int read_result = fscanf(file,"%[^\n]\n",buffer);
    if(read_result<0){ // read a line from the file
      continue;
    }
    double vec[d];
    char * token = strtok(buffer, " ");
    // printf("NAME = %s\n",token);
    char name[MAX_INPUT_LENGTH];
    strcpy(name,token);
    name[strlen(name)]='\0';
    token = strtok(NULL, " ");
     // loop through the string to extract all other tokens
     int counter = 0;
     while( token != NULL ) {
        // printf( " - %s\n", token ); //printing each token
        vec[counter++]=atof(token);
        token = strtok(NULL, " ");
     }
     Vector vecTmp=initVector(vec,name);
     insertToHyperCube(hc,vecTmp);
     (*inputs) = listInsert((*inputs),vecTmp,-1);



  }

  fclose(file);
}



void readQueryFile(char* queryFile,char* outputFile,HyperCube hc,List inputs,int n,double radius,int hammingDist,int m){

   FILE *file = fopen(queryFile, "r"); // read mode

   if (file == NULL){
      perror("Error while opening the file.\n");
      exit(-1);
   }

  if (feof(file)){ // empty file, return
    return;
  }
  FILE* fptr;
  fptr = fopen(outputFile, "w");
  if(fptr == NULL){
    /* File not created hence exit */
    printf("Unable to create file.\n");
    exit(EXIT_FAILURE);
  }

  char buffer[MAX_INPUT_LENGTH];
  Vector nearest=NULL;
  Vector nNearest[n];
  double nearestDist=-1;
  double knearestDists[n];
  double vec[d];

  while(!feof(file)){
    fflush(stdin);  // clear stdin buffer
    if(fscanf(file,"%[^\n]\n",buffer)<0){ // read a line from the file
      continue;
    }

    for (int i = 0; i < n; i++){
      nNearest[i]=NULL;
      knearestDists[i]=-1;
    }

    int id;
    char * token = strtok(buffer, " ");
    id=atoi(token);
    char name[MAX_INPUT_LENGTH];
    strcpy(name,token);
    token = strtok(NULL, " ");
     // loop through the string to extract all other tokens
     int counter = 0;
     while( token != NULL ) {
        // printf( " - %s\n", token ); //printing each token
        vec[counter++]=atof(token);
        token = strtok(NULL, " ");
     }
     Vector vecTmp=initVector(vec,name);
     fprintf(fptr, "Query %d:\n",id);
     // nearestNeigborHypercube(hc,vecTmp,1,4,fptr);
     // printf("================================================\n");
     clock_t begin_true = clock();
     listFindKNearestNeighbors(inputs,vecTmp,nNearest,knearestDists,d,n,-1);
     clock_t end_true = clock();
     double time_spent_true = (double)(end_true - begin_true) / CLOCKS_PER_SEC;

     clock_t begin_cube = clock();
     kNearestNeigborsHypercube(hc,vecTmp,n,hammingDist,m,knearestDists,fptr);
     clock_t end_cube = clock();
     double time_spent_cube = (double)(end_cube - begin_cube) / CLOCKS_PER_SEC;
     fprintf(fptr, "tCube: %f seconds\n",time_spent_cube);
     fprintf(fptr, "tTrue: %f seconds\n",time_spent_true);
     // printf("================================================\n");
     clock_t begin_radius = clock();
     radiusNeigborHypercube(hc,vecTmp,radius,hammingDist,m,fptr);
     clock_t end_radius = clock();
    double time_spent_radius = (double)(end_radius - begin_radius) / CLOCKS_PER_SEC;
      fprintf(fptr, "tRadiusSearch: %f seconds\n\n\n",time_spent_radius);
     // printLSH(temp);
     deleteVector(vecTmp);
  }
  fclose(fptr);
  fclose(file);
}
