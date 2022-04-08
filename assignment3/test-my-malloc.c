/*
 * test-my-malloc.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define ALLOC_SIZE 24

int
main(int argc, char *argv[])
{

    write(1, "Enter three strings to test malloc:\n", 37);

    char *a;
    char *b;
    char *c;
    char *d;

    while(1 != 0){
        a = malloc(ALLOC_SIZE);
        b = malloc(ALLOC_SIZE);
        c = malloc(ALLOC_SIZE);
        d = malloc(ALLOC_SIZE);


        fgets(a, ALLOC_SIZE, stdin);
        fgets(b, ALLOC_SIZE, stdin);
        fgets(c, ALLOC_SIZE, stdin);
        fgets(d, ALLOC_SIZE, stdin);

        write(1, "================================================\n", 50);
        write(1, "String A repeated from malloc: ", 30);
        write(1, a, ALLOC_SIZE);
        write(1, "String B repeated from malloc: ", 30);
        write(1, b, ALLOC_SIZE);
        write(1, "String C repeated from malloc: ", 30);
        write(1, c, ALLOC_SIZE);
        write(1, "String D repeated from malloc: ", 30);
        write(1, d, ALLOC_SIZE);
        write(1, "\n", 1);
        write(1, "================================================\n", 50);

        free(d);
        free(b);
        free(a);
        free(c);
    }
}
