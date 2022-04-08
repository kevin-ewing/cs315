/*
 * myio.h
 */

#ifndef __MYIO_H
#define __MYIO_H

FILE *myopen(const char *restrict pathname, const char *restrict mode);
int myclose(FILE *stream);
size_t myread(void *restrict ptr, size_t size, size_t nmemb,
    FILE *restrict stream);
size_t mywrite(const void *restrict ptr, size_t size, size_t nitems, 
    FILE *restrict stream);
int myseek(FILE *stream, long offset, int whence);
int myflush(FILE *stream);

#endif /* __MYIO_H */
