#include "parsing.h"

int countLines(const FILE *fp){
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

int** readFile(FILE *file,int dimensions){
  if (feof(file)){ // empty file, return
    return;
  }

  // int numberOfVectors=countLines(file);
  // int** arrVectors = malloc(numberOfVectors * sizeof(int*));
  // for (i = 0; i < numberOfVectors; i++)
  //     arrVectors[i] = malloc(dimensions * sizeof(int));

  char buffer[MAX_INPUT_LENGTH];
  char skipWord[100];


  while(!feof(file)){
    fflush(stdin);  // clear stdin buffer
    if(fscanf(file,"%[^\n]\n",buffer)<0){ // read a line from the file
      continue;
    }
    sscanf(buffer,"%s",);

  }

    // for (int i = 0; i < numberOfVectors; i++) {
    //   for (int i = 0; i < dimensions; i++) {
    //
    //   }
    // }

}
