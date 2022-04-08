/*
 * test_myio.c
 */

#include <stdlib.h>
#include <stdio.h>
#include "myio.h"

#define BYTES_PER_ITERATION 10240

int
main(int argc, char *argv[])
{
    FILE *in, *out;
    char *src_filename, *dst_filename;
    size_t bytes_read, bytes_written;
    char buf[BYTES_PER_ITERATION];

    /* check command-line arguments */
    if(argc != 3) {
        printf("usage: %s source dest\n", argv[0]);
        exit(1);
    }
    src_filename = argv[1];
    dst_filename = argv[2];

    /* open files */
    in = myopen(src_filename, "r");
    if(in == NULL) {
        perror("myopen");
        exit(2);
    }

    if((out = myopen(dst_filename, "w")) == NULL) {
        perror("myopen");
        exit(3);
    }

    /* do the copy */
    while((bytes_read = myread(buf, 1, BYTES_PER_ITERATION, in)) > 0) {
        bytes_written = mywrite(buf, 1, bytes_read, out);
        if(bytes_written == 0) {
            perror("mywrite");
            exit(4);
        }
    }

    /* the while loop above could terminate if myread encountered an error, so
     * check that and report if necessary */
    if(ferror(in)) {
        perror("myread");
        exit(5);
    }

    /* clean up */
    myclose(out);
    myclose(in);
}
