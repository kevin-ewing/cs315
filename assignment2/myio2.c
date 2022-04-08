/*
 * myio.c
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PAGE_SIZE 1024
#define false 0
#define true 1


struct my_file {
    int file_descriptor;
    void * stream;
};


int myopen(const char *restrict pathname, const char *restrict mode);
int myclose(struct my_file * stream);
int myread(void *restrict ptr, int size, int nmemb, struct my_file * stream);
int mywrite(const void *restrict ptr, int size, int nitems, struct my_file * stream);
int myseek(struct my_file * stream, long offset, int whence);
int myflush(struct my_file * stream);


void * buffer;


//ACTUAL DEFS
int myopen(const char *restrict pathname, const char *restrict mode) {
    // struct my_file * returned_file;
    // int file_descriptor;

    // if (strcmp (mode, "r")){
    //     printf("r");
    //     file_descriptor = open(pathname, O_RDONLY);
    // }
    // if (strcmp (mode, "w")){
    //     printf("w");
    //     file_descriptor = open(pathname, O_WRONLY | O_CREAT | O_TRUNC);
    // }
    // if (strcmp (mode, "a")){
    //     printf("a");
    //     file_descriptor = open(pathname, O_WRONLY | O_CREAT | O_APPEND);
    // }
    // if (strcmp (mode, "r+")){
    //     printf("r+");
    //     file_descriptor = open(pathname, O_RDWR);
    // }
    // if (strcmp (mode, "w+")){
    //     printf("w+");
    //     file_descriptor = open(pathname, O_RDWR | O_CREAT | O_TRUNC);
    // }
    // if (strcmp (mode, "a+")){
    //     printf("a+");
    //     file_descriptor = open(pathname, O_RDWR | O_CREAT | O_APPEND);
    // }
    // else{
    //     return NULL;
    // }

    // returned_file->file_descriptor = file_descriptor;
    // returned_file->stream ;

    // return returned_file;

    // return fopen(pathname, mode);
}

int myclose(struct my_file *stream){
    return fclose(stream);
}

int myread(void *restrict ptr, int size, int nmemb, struct my_file * stream){
    stream->file_descriptor
}

int mywrite(const void *restrict ptr, int size, int nitems, struct my_file * stream){
    return fwrite(ptr, size, nitems, stream);
}

int myseek(struct my_file *stream, long offset, int whence){
    return fseek(stream, offset, whence);
}

int myflush(struct my_file *stream){
    return fflush(stream);
}
