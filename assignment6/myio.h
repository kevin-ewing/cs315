/*
 * myio.h
 */

#ifndef __MYMMAPIO_H
#define __MYMMAPIO_H

struct file {
    int file_descriptor;
    long file_size;
    int position;
    int mapped;
    char * map;
};

struct file * myopen(const char *pathname, int flags);
int myclose(struct file *stream);
int myread(struct file *stream, void *ptr, int count);
int mywrite(struct file *stream, void *ptr, int count);
int myseek(struct file *stream, long offset, int whence);
int myflush(struct file *stream);

#endif /* __MYMMAPIO_H */
