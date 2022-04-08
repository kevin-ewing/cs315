/*
 * playground.c
 *
 * Implementation of a shell pipe
 */

#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

int main () {
  char input_stream[4096];

  /* Stay in the shell until "exit" has been entered */
  while(1){

    /* Get input from standard in */
    if (fgets(input_stream, 4096, stdin) == 0){
        printf("\n");
        break;
    }

    /* Print new prompt if the only input is a new line */
    if(strcmp(input_stream, "\n") == 0){
        printf("\n");
    }
    else{
      char * split_input = strtok(input_stream, "|");

      while(split_input){
        printf("split_input: %s\n", split_input);
        










        split_input = strtok(NULL, "|");
      }      
    }        
  }
}