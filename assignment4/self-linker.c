/*
 * self-linker.c
 */

 #include <unistd.h>
 #include <string.h>
 #include <errno.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <limits.h>
 #include <sys/wait.h>
 #include <dlfcn.h>
 #include <gnu/lib-names.h>
 #include <dlfcn.h>
 #include <stdint.h>

/* Definitions */
 #define PUTS_GOT_ADDRESS 0x555555558018
 #define PUTS_LIBC_ADDRESS 0x00007ffff7e5dab0
 #define PUTS_PLT_ADDRESS 0x555555555030

/* Variables */
extern void *_GLOBAL_OFFSET_TABLE_;

/*Prototyping*/
void *dlopen(const char *filename, int flags);
int dlclose(void *handle);
void a_link_to_the_puts();

void a_link_to_the_puts(){
  void *handle;
  char *error;

  handle = dlopen(LIBC_SO, RTLD_LAZY); /*Dynamically load libc and store its handle in variable handle*/
  if (!handle) { /*Double-check that we have actually loaded libc and returned a handle*/
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  dlerror();    /* Clear any existing errors */


  *(void **) (&_GLOBAL_OFFSET_TABLE_ + 3) = dlsym(handle, "puts"); /*Modify the GOT like a boss*/

  error = dlerror();
  if (error != NULL) { /*If dlerror() finds any errors, exit and report them*/
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }
  dlclose(handle); /*Decrement the reference count of libc*/
}

int main(void) {
  a_link_to_the_puts();
  puts("I am the senate."); /*Call puts twice, but even in the first call, we have already linked it so the linker is not called*/
  puts("Not yet.");
  return 0;
}
