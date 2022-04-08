/*
 * copy-file.c
 *
 * usage: ./copy-file source-file destination-file
 */

#include <stdio.h>
#include <stdlib.h>

#define BYTES_PER_ITERATION 200

void compare_files(char *file_path1, char *file_path2);


int
main(int argc, char *argv[])
{
    FILE *in, *out;
    char *src_filename, *dst_filename;
    int bytes_read, bytes_written;
    char buf[BYTES_PER_ITERATION];

    compare_files("testing/copy_to1", "testing/copy_to1");
    
}

void compare_files(char *file_path1, char *file_path2){
    FILE *fp1 = fopen(file_path1, "r");
    FILE *fp2 = fopen(file_path2, "r");
  
    if (fp1 == NULL || fp2 == NULL)
    {
       printf("Error : Files not open");
       exit(0);
    }

    char ch1 = getc(fp1);
    char ch2 = getc(fp2);
  
    int error = 0, pos = 0, line = 1;
  
    while (ch1 != EOF && ch2 != EOF)
    {
        pos++;
        if (ch1 == '\n' && ch2 == '\n')
        {
            line++;
            pos = 0;
        }
        if (ch1 != ch2)
        {
            error++;
            printf("Line Number : %d \tError"
               " Position : %d \n", line, pos);
        }
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    }

    fclose(fp1);
    fclose(fp2);

    if (error == 0){
        printf("\t\033[0;32mTEST PASSED\033[0m: output equal\n");
    }
    else{
        printf("\t\033[0;33mTEST FAILED\033[0m: output different\n");
    }
}