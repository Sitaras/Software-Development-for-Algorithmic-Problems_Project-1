#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Vector/vector.h"
#include "../hashTable/hashTable.h"
#include "../LSH/lsh.h"

#define OUT    0
#define IN    1
#define FALSE    0
#define TRUE    1

#define MAX_INPUT_LENGTH 1024

extern int d;

// returns number of words in str
int countWords(char *str){
    int state = OUT;
    int wc = 0;  // word count

    // Scan all characters one by one
    while (*str)
    {
        // If next character is a separator, set the
        // state as OUT
        if (*str == ' ' || *str == '\n' || *str == '\t')
            state = OUT;

        // If next character is not a word separator and
        // state is OUT, then set the state as IN and
        // increment word count
        else if (state == OUT)
        {
            state = IN;
            ++wc;
        }

        // Move to next character
        ++str;
    }

    return wc;
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


// void readFile(char* fileName,LSH lsh){
//
//    FILE *file = fopen(fileName, "r"); // read mode
//
//    if (file == NULL){
//       perror("Error while opening the file.\n");
//       exit(-1);
//    }
//
//   if (feof(file)){ // empty file, return
//     return;
//   }
//
//   // int numberOfVectors=countLines(file);
//   // int** arrVectors = malloc(numberOfVectors * sizeof(int*));
//   // for (i = 0; i < numberOfVectors; i++)
//   //     arrVectors[i] = malloc(dimensions * sizeof(int));
//
//   char buffer[MAX_INPUT_LENGTH];
//
//
//   while(!feof(file)){
//     fflush(stdin);  // clear stdin buffer
//     if(fscanf(file,"%[^\n]\n",buffer)<0){ // read a line from the file
//       continue;
//     }
//
//     double vec[d];
//     char * token = strtok(buffer, "  ");
//     printf("NAME = %s\n",token);
//     token = strtok(NULL, "  ");
//      // loop through the string to extract all other tokens
//      int counter = 0;
//      while( token != NULL ) {
//         printf( " - %s\n", token ); //printing each token
//         vec[counter++]=atof(token);
//         token = strtok(NULL, "  ");
//      }
//      Vector vecTmp=initVector(vec);
//      insertToLSH(lsh,vecTmp);
//
//
//
//
//   }
//
//
//   fclose(file);
//
//
// }
