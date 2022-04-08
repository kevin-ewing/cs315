/*
 * mymmapio.c
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
#include "myio.h"

#define WRITE_INCREMENT 1000

struct file * myopen(const char *pathname, int flags){
    /* opens a file and initializes a file struct corresponding to it returns a pointer to that struct*/
    int file_descriptor;
    struct file *returned_file;
    struct stat file_info;
    int filter = O_TRUNC | O_CREAT;


    /* verifies malloc */
    if((returned_file = malloc(sizeof(struct file))) == 0){
        return NULL;
    }

    /* opens the file */
    if((file_descriptor = open(pathname, (flags & filter) | O_RDWR, (mode_t) 0664)) == -1){
        return NULL;
    }

    /* gets stats on the file */
    if(fstat(file_descriptor, &file_info) == -1){
        return NULL;
    }

    /* initializes all of the fields */
    returned_file->file_descriptor = file_descriptor;
    returned_file->file_size = file_info.st_size;
    returned_file->position = 0;
    returned_file->mapped = 0;

    if(returned_file->file_size != 0){
        if((returned_file->map = mmap(0, returned_file->file_size, PROT_READ, MAP_SHARED, returned_file->file_descriptor, 0)) == MAP_FAILED){
            return NULL;
        }
    }

    return returned_file;
}



int myread(struct file *stream, void *ptr, int count){
    /* reads from the file pointed at by stream, and writes to ptr, count number of bytes */

    if(stream->file_size == 0){
        return 0;
    }

    if(stream->position + count > stream->file_size){
        count = stream->file_size - stream->position;
    }
    
    if(stream->position == stream->file_size){
        return 0;
    }

    /* Copy over the data */
    memcpy(ptr, stream->map + stream->position, count);

    /* increment pointers */
    stream->position += count;

    return count;
}

int mywrite(struct file *stream, void *ptr, int count){
    /* writes from the file pointed at by stream, and reads to ptr, count number of bytes */
    int next_file_size, added_size;
    int file_size = stream->file_size;

    added_size = count;
    next_file_size = stream->file_size + added_size;

    /* if the file needs to be increased in size */
    if(file_size < stream->position + count){

        /* will group together calls by adding more than asked for */
        if (added_size < WRITE_INCREMENT){
            next_file_size = stream->file_size + WRITE_INCREMENT;
        }
        else {
            next_file_size = stream->file_size + added_size;
        }

        /* unmap the previous too small mapping */
        if(stream->file_size != 0){
            if(munmap(stream->map, stream->file_size) == -1){
                return -1;
            }
        }

        /* change the file size */
        if(ftruncate(stream->file_descriptor, next_file_size) == -1){
            return -1;
        }

        /* only need to re mmap when the file size has changed */
        if((stream->map = mmap(0, next_file_size, PROT_READ | PROT_WRITE, MAP_SHARED, stream->file_descriptor, 0)) == MAP_FAILED){
            return -1;
        }
    }
    else{
        next_file_size = stream->file_size;
    }

    /* do the actual writing */
    memcpy(stream->map + stream->position, (char*) ptr, added_size);

    /* increment pointers */
    stream->position += count;
    stream->file_size = next_file_size;

    return added_size;
}

int myclose(struct file *stream){
    int return_value;

    /* close the fd associated with the stream */
    if((return_value = close(stream->file_descriptor)) == -1){
        return return_value;
    }

    /* undo the map associated with the stream */
    if(stream->file_size != 0){
        if(munmap(stream->map, stream->file_size) == -1){
            return -1;
        }
    }

    /* free the stream data */
    free(stream);    
    return return_value;
}


int myseek(struct file *stream, long offset, int whence){
    /* all myseek will need to do is change the position in the file struct*/
    if(whence == SEEK_SET){
        return stream->position = offset;
    }
    else if(whence == SEEK_CUR){
        return stream->position += offset;
    }
    else{
        return -1;
    }
}

int myflush(struct file *stream){
    /* no buffer, so flush is a bit unneeded */
    return 0;
}