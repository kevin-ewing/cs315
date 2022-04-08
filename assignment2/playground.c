/*
 * playground.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>


int
main(int argc, char *argv[])
{
    FILE *file;
    char *filename;

    filename = "test_files/test_from.txt";

    file = fopen(filename, "r");

    /* open files */
    printf("File pointer: %p\n", file);
    printf("File pointer: %d\n", file->_fileno);
    printf("File offset: %d\n", file->_offset);
    printf("File flag: %x\n", file->_flags);

}
