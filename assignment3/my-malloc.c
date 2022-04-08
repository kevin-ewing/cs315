/*
 * my-my_malloc.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdint.h>

#define INTERNAL_SIZE_T size_t
#define TRUE 1
#define FALSE 0
#define ALIGNMENT_SIZE 16
#define PAGE_SIZE 800

int total_break_space = 0;

struct allocation {
    int size; /*Size of this allocation*/
    struct allocation * next; /*Pointer to the next allocation*/
    struct allocation * prev; /*Pointer to the previous allocation*/
    char padding[8];
};

/*Prototypes*/
void my_free(void *ptr);
void * my_malloc(size_t size);
void * calloc(size_t count, size_t size);
void * realloc(void *ptr, size_t size);
long check_space(int request_size);
void make_allocation(struct allocation *prev_allocation, int allocation_size, struct allocation *next_allocation);

struct allocation *head;
struct allocation *break_start;


/****************************************************************************************
*
* TO MOVE TO TEST FILE 
*
****************************************************************************************/
#define ALLOC_SIZE 500

int
main(int argc, char *argv[])
{

    write(1, "Enter three strings to test my_malloc:\n", 37);

    char *a;
    char *b;
    char *c;
    char *d;

    // d = my_malloc(ALLOC_SIZE);

    // while(1 != 0){
    //     a = my_malloc(ALLOC_SIZE);
    //     b = my_malloc(ALLOC_SIZE);
    //     c = my_malloc(ALLOC_SIZE);

    //     fgets(a, ALLOC_SIZE, stdin);
    //     fgets(b, ALLOC_SIZE, stdin);
    //     fgets(c, ALLOC_SIZE, stdin);
    //     fgets(d, ALLOC_SIZE, stdin);

    //     printf("String A repeated from my_malloc: %s", a);
    //     printf("String B repeated from my_malloc: %s", b);
    //     printf("String C repeated from my_malloc: %s", c);
    //     printf("String D repeated from my_malloc: %s", d);
        
    //     my_free(d);
    //     my_free(c);
    //     my_free(a);
    //     d = my_malloc(ALLOC_SIZE);
    //     my_free(b);
        
  
    // }


    while(1 != 0){
        a = my_malloc(ALLOC_SIZE);
        b = my_malloc(ALLOC_SIZE);
        c = my_malloc(ALLOC_SIZE);
        d = my_malloc(ALLOC_SIZE);

        fgets(a, ALLOC_SIZE, stdin);
        fgets(b, ALLOC_SIZE, stdin);
        fgets(c, ALLOC_SIZE, stdin);
        fgets(d, ALLOC_SIZE, stdin);

        printf("String A repeated from my_malloc: %s", a);
        printf("String B repeated from my_malloc: %s", b);
        printf("String C repeated from my_malloc: %s", c);
        printf("String D repeated from my_malloc: %s", d);
        
        my_free(a);
        my_free(d);
        my_free(b);
        my_free(c);
        
  
    }
}
/****************************************************************************************
*
* TO MOVE TO TEST FILE 
*
****************************************************************************************/


void my_free(void *ptr){
    struct allocation *freed_allocation;

    /* Free, when given a null pointer, does nothing */
    if (ptr != NULL){

        /* Get to the beginning of the metadata before the allocation by casting to alloc struct and subtracting one */
        freed_allocation = ((struct allocation *) ptr) -1;

        if (freed_allocation->prev != NULL && freed_allocation->next != NULL){
            /* yes prev and yes next */
            write(1, "FREE=CASE=1=====================================\n", 50);
            freed_allocation->prev->next = freed_allocation->next;
            freed_allocation->next->prev = freed_allocation->prev;
        }
        else if (freed_allocation->prev == NULL && freed_allocation->next != NULL){
            /* no prev and yes next */
            write(1, "FREE=CASE=2=====================================\n", 50);
            head = freed_allocation->next;
            freed_allocation->next->prev = NULL;
        }
        else if(freed_allocation->prev != NULL && freed_allocation->next == NULL){
            /* yes prev and no next */
            write(1, "FREE=CASE=3=====================================\n", 50);
            freed_allocation->prev->next = NULL;
        }
        else{
            /* no prev and no next */
            write(1, "FREE=CASE=4=====================================\n", 50);
            head = NULL;
        }
    }
}

void * my_malloc(size_t size){
    /* Returns a void pointer to the start of the allocated memory chunk just as my_malloc does */ 

    int remainder_size = size % ALIGNMENT_SIZE;

    return (void *)(check_space(sizeof(struct allocation) + size + (ALIGNMENT_SIZE-remainder_size)));
}

void * calloc(size_t count, size_t size){
    /* Returns an array of void pointers to the start of the allocated memory chunk just as calloc does */

    if (count == 0 || size == 0){
        return NULL;
    }
    else {
        if (count > (INT_MAX - size)) {
            return NULL;
        }
        else{
            return my_malloc(count*size);
        }
    }
}

void * realloc(void *ptr, size_t new_size){
  struct allocation *re_allocation;
  /* Get to the beginning of the metadata before the reallocation */
  re_allocation = (struct allocation *) (char *) ptr - sizeof(struct allocation);
     if (ptr == NULL){
       return my_malloc(new_size);
     }
    else if(new_size == 0){
      my_free(ptr);
      return ptr;
    }
    else if((re_allocation->size) > new_size){
      re_allocation->size = new_size;
      return ptr;
    }
    else{
      /*Unless there's a way to actually 'grow' our memory, this is the best solution I could think of*/
      my_free(ptr);
      return my_malloc(new_size);
    }
}


long check_space(int request_size) {
    /* Returns a long integer of the pointer to the start of the allocated memory*/

    struct allocation *curr_allocation;
    

    /* if head has not been initialized to be the first allocation pointer */
    if (head == NULL && total_break_space == 0) {

        write(1, "HEADLESS=CASE=INCBRK============================\n", 50);


        head = sbrk(0); /*initialize the head to be at the bottom of the stack*/
        break_start = head;

        /*If the requested size is larger then the current total space*/
        if (request_size > PAGE_SIZE){
            int remainder_size = request_size % PAGE_SIZE;
            sbrk(request_size + remainder_size);
            total_break_space += request_size + remainder_size;
        }
        else {
            sbrk(PAGE_SIZE);
            total_break_space += PAGE_SIZE;
        }

        /* Return a pointer to the start of the stack, i.e. where the head has
        been initialized to */
        make_allocation(NULL, request_size, NULL);
        return (long) head + sizeof(struct allocation);
    }
    else if (head == NULL && total_break_space != 0) {
        write(1, "HEADLESS=CASE=NO=INCBRK=========================\n", 50);

        /*If the requested size is larger then the current total space*/
        
        if (request_size > total_break_space){
            if (request_size > PAGE_SIZE){
                int remainder_size = request_size % PAGE_SIZE;
                sbrk(request_size + remainder_size);
                total_break_space += request_size + remainder_size;
            }
            else {
                sbrk(PAGE_SIZE);
                total_break_space += PAGE_SIZE;
            }
        }

        /* Return a pointer to the start of the stack, i.e. where the head has
        been initialized to */
        head = break_start;
        make_allocation(NULL, request_size, NULL);
        return (long) head + sizeof(struct allocation);
    }
    else{
        /* When the has been initialized we need to iterate through the entire LL
        * and check if there is enough space between, pointers or at the end of
        * the last pointer */

        curr_allocation = head;

        /* Traversing the LL*/
        while (curr_allocation->next != NULL){    
            /* check if there is enough space between the pointer to the
            current allocation iterator + its size to the next allocation */

            /*ADDING A ALLOCATION WITHIN A GAP IN THE LL, MEANING THERE ARE ITEMS AFTER THE NEW ALLOCATION*/
            if ((curr_allocation + curr_allocation->size - curr_allocation->next > request_size)){
                
                write(1, "MIDDLE=CASE=====================================\n", 50);

                make_allocation(curr_allocation, request_size,curr_allocation->next);
                return (long)curr_allocation + curr_allocation->size + sizeof(struct allocation);
            }
            else {
                curr_allocation = curr_allocation->next;
            }

        }

        /* see if there is enough space left in the buffer after the
        last allocation to store new allocation */

        /* IF WE HAVE ITERATED TO END AND THERE IS SPACE IN THE BREAK TO APPEND A ALLOCATION*/
        if (curr_allocation - head + curr_allocation->size + request_size < total_break_space){
        
            write(1, "END=CASE========================================\n", 50);

            make_allocation(curr_allocation, request_size, NULL);
            return (long)curr_allocation + curr_allocation->size + sizeof(struct allocation);
        }
        else {
            /* IF WE HAVE ITERATED TO END AND THERE IS NOT SPACE IN THE BREAK TO APPEND A ALLOCATION*/

            /*If the requested size is larger then the current total space*/
            if (request_size > PAGE_SIZE){
                int remainder_size = request_size % PAGE_SIZE;
                sbrk(request_size + remainder_size);
                total_break_space += request_size + remainder_size;
            }
            else {
                sbrk(PAGE_SIZE);
                total_break_space += PAGE_SIZE;
            }

            
            write(1, "END=CASE=ADD=SPACE==============================\n", 50);

            make_allocation(curr_allocation, request_size, NULL);
            return (long)curr_allocation + curr_allocation->size + sizeof(struct allocation); /*Meaning there is not enough space in 
            the current list with buffer size to put the requested space*/
        }
        return 0;
    }
}

void make_allocation(struct allocation *prev_allocation, int request_size, struct allocation *next_allocation){
    /*Creates the new allocation and link it into the linked list*/
    struct allocation *curr_allocation;

    /*If the prvious and next alocations are null*/
    if (prev_allocation == NULL && next_allocation == NULL){
        write(1, "ADD=CASE=1======================================\n", 50);
        curr_allocation = head;

        /*Assignments*/
        curr_allocation->prev = NULL;
        curr_allocation->size = request_size;
        curr_allocation->next = NULL;
    }
    /*If the prvious alocation is not null and the next is*/
    else if (prev_allocation != NULL && next_allocation == NULL) {
        write(1, "ADD=CASE=2======================================\n", 50);
        curr_allocation = (struct allocation *)((char *) prev_allocation + prev_allocation->size);

        /*Assignments to curr*/
        curr_allocation->prev = prev_allocation;
        curr_allocation->size = request_size;
        curr_allocation->next = NULL;

        /*Assignments to prev*/
        prev_allocation->next = curr_allocation;
    }
    /*If the prvious alocation is null and the next is not*/
    else if (prev_allocation == NULL && next_allocation != NULL) {
        write(1, "ADD=CASE=3======================================\n", 50);
        curr_allocation = (struct allocation *)((char *) prev_allocation + prev_allocation->size);

        /*Assignments to curr*/
        curr_allocation->prev = NULL;
        curr_allocation->size = request_size;
        curr_allocation->next = next_allocation;

        /*Assignments to next*/
        next_allocation->prev = curr_allocation;
    }
    else {
    /*If the prvious and next alocations are not null */
        write(1, "ADD=CASE=4======================================\n", 50);
        curr_allocation = (struct allocation *)((char *) prev_allocation + prev_allocation->size);

        /*Assignments to curr*/
        curr_allocation->prev = prev_allocation;
        curr_allocation->size = request_size;
        curr_allocation->next = next_allocation;

        /*Assignments to prev*/
        prev_allocation->next = curr_allocation;
        /*Assignments to next*/
        next_allocation->prev = curr_allocation;
    }
}
