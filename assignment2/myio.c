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
#include <sys/mman.h>
#include <stdlib.h>

#define PAGE_SIZE 1024
#define false 0
#define true 1
#define BYTES_PER_ITERATION 10

struct my_file {
    int file_descriptor;
    int read_position;
    int write_position;
    int file_size;
};

struct my_file *myopen(const char *restrict pathname, const char *restrict mode);
int myclose(struct my_file *stream);
int myread(void *restrict ptr, int size, int nitems, struct my_file *stream);
int mywrite(void *restrict ptr, int size, int nitems, struct my_file *stream);
int myseek(struct my_file *stream, long offset, int whence);
int myflush(struct my_file *stream);

int main (int argc, char *argv[]) {
    char *ptr = malloc(BYTES_PER_ITERATION);
    struct my_file *in, *out;
    char *src_filename, *dst_filename;
    int bytes_read, bytes_written;

    src_filename = "testing/copy_from";
    dst_filename = "testing/copy_to";

    if((in = myopen(src_filename, "w+")) == NULL) {
        perror("myopen");
        exit(3);
    }

    if((out = myopen(dst_filename, "w+")) == NULL) {
        perror("myopen");
        exit(3);
    }

    while((bytes_read = myread(ptr, 1, BYTES_PER_ITERATION, in)) > 0) {
        bytes_written = mywrite(ptr, 1, bytes_read, out);
        if(bytes_written == 0) {
            perror("mywrite");
            exit(4);
        }
    }

    if(myclose(in) != 0) {
        perror("myclose");
        exit(1);
    }
    if(myclose(out) != 0) {
        perror("myclose");
        exit(1);
    }
}


struct my_file * myopen(const char *restrict pathname, const char *restrict mode) {
    int file_descriptor;
    struct my_file *returned_file = malloc(sizeof(struct my_file));
    struct stat file_info;
    
    int flag_bits = 0;

    if (strcmp (mode, "r") ==0 ){
        flag_bits = O_RDONLY;
    }
    else if (strcmp (mode, "w") ==0 ){
        flag_bits = O_WRONLY | O_CREAT | O_TRUNC;
    }
    else if (strcmp (mode, "a") ==0 ){
        flag_bits = O_WRONLY | O_CREAT | O_APPEND;
    }
    else if (strcmp (mode, "r+") ==0 ){
        flag_bits = O_RDWR;
    }
    else if (strcmp (mode, "w+") ==0 ){
        flag_bits = O_RDWR | O_CREAT | O_TRUNC;
    }
    else if (strcmp (mode, "a+") ==0 ){
        flag_bits = O_RDWR | O_CREAT | O_APPEND;
    }
    else{
        return NULL;
    }

    if ((file_descriptor = open(pathname, flag_bits, (mode_t) 0664)) == -1) {
        perror("open");
        exit(1);
    }

    if (fstat(file_descriptor, &file_info) == -1) {
        perror("fstat");
        exit(1);
    }

    returned_file->file_descriptor = file_descriptor;
    returned_file->file_size = file_info.st_size;
    returned_file->read_position = 0;
    returned_file->write_position = 0;

    return returned_file;
}


int myread(void *restrict ptr, int size, int nitems, struct my_file *stream){
    int file_descriptor = stream->file_descriptor;
    int file_size = stream->file_size;
    int map_size;
    int request_map_size = size * nitems;
    

    if (read(file_descriptor, ptr, file_size) == -1) {
        perror("read");
        exit(1);
    }

    return map_size;
}

int mywrite(void *restrict ptr, int size, int nitems, struct my_file *stream){
    int prev_file_size, next_file_size;
    int file_descriptor = stream->file_descriptor;
    int file_size = stream->file_size;
    int write_size = size * nitems;
    struct my_file *write_buffer = malloc(BYTES_PER_ITERATION);


    if (write(file_descriptor, ptr, write_size) == -1){
        perror("write");
        exit(1);
    }

    return write_size;
}

int myclose(struct my_file *stream){
    int return_value;

    if ((return_value = close(stream->file_descriptor)) == -1){
        perror("close");
        return return_value;
    }
    
    free(stream);
    
    return return_value;
}


int myseek(struct my_file *stream, long offset, int whence){
    int result;
    int file_descriptor = stream->file_descriptor;

    result = lseek (file_descriptor, offset, whence);
    return 0;
}


int myflush(struct my_file *stream){
    return 0;
}