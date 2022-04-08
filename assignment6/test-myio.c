/*
 * test-myio.c
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

void compare_files(char *file_path1, char *file_path2);
void simple_copy_sequence (char * src_filename, char * dst_filename1, char * dst_filename2, int bytes_per_iter);
void seek_copy_read(char * src_filename, char * dst_filename1, char * dst_filename2, int bytes_per_iter, int offset, int whence);
void seek_copy_write(char * src_filename, char * dst_filename1, char * dst_filename2, int bytes_per_iter, int offset, int whence);
void test_flush(char * src_filename, char * dst_filename1, char * dst_filename2, int bytes_per_iter);

int main (int argc, char *argv[]) {
    char *src_filename1, *src_filename2, *src_filename3;
    char *dst_filename1, *dst_filename2;

    src_filename1 = "testing/copy_from1";
    src_filename2 = "testing/copy_from2";
    src_filename3 = "testing/copy_from3";
    dst_filename1 = "testing/copy_to1";
    dst_filename2 = "testing/copy_to2";

    /* Test 1 simple copy over from one file to another with an overly large buffer, so
    * that the file will all be coppied in one buf */
    simple_copy_sequence(src_filename1, dst_filename1, dst_filename2, 100000);
    printf("Simple copy file sequence: overly large buffer\n");
    compare_files(dst_filename1, dst_filename2);

    /* Test 2 simple copy over from one file to another with a buffer of length 1 
    * also note that it is coppying onto an already existing file */
    simple_copy_sequence(src_filename1, dst_filename1, dst_filename2, 1);
    printf("Simple copy file sequence: iteration size 1 and writing to pre-existing file\n");
    compare_files(dst_filename1, dst_filename2);

    /* Test 3 simple copy over from empty file to another ensures that mmapio does not error */
    simple_copy_sequence(src_filename2, dst_filename1, dst_filename2, 300);
    printf("Simple copy file sequence: empty src file\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }

    /* Test 4 copy over from one file to another after a seek set */
    seek_copy_read(src_filename1, dst_filename1, dst_filename2, 300, 200, SEEK_SET);
    printf("Seek set on src, copy file sequence: normal seek offset\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }

    /* Test 5 copy over from one file to another after a seek set to offset of 0, ensure no error*/
    seek_copy_read(src_filename1, dst_filename1, dst_filename2, 300, 0, SEEK_SET);
    printf("Seek set on src, copy file sequence: no seek offset\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }

    /* Test 6 copy over from one file to another after a seek curr to reasonable offset */
    seek_copy_read(src_filename1, dst_filename1, dst_filename2, 300, 500, SEEK_CUR);
    printf("Seek cur on src, copy file sequence: large seek offset\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }


    /* Test 7 copy over from one file to another after a seek set in destination file, ensure no error**/
    seek_copy_write(src_filename1, dst_filename1, dst_filename2, 300, 200, SEEK_SET);
    printf("Seek set on dst, copy file sequence: normal seek offset\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }

    /* Test 8 copy over from one file to another after a seek set in destination file of offset 0, 
    * ensure no error**/
    seek_copy_write(src_filename1, dst_filename1, dst_filename2, 300, 0, SEEK_SET);
    printf("Seek set on dst, copy file sequence: no seek offset\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }

    /* Test 9 copy over from one file to another after a seek cur in destination file, ensure no error**/
    seek_copy_write(src_filename1, dst_filename1, dst_filename2, 300, 500, SEEK_CUR);
    printf("Seek cur on dst, copy file sequence: large seek offset\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }

    /* Test 10 copy over with buffer flush after each read write, buffer size medium*/
    test_flush(src_filename1, dst_filename1, dst_filename2, 200);
    printf("Flush after every write, copy file sequence: medium buffer size\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }

    /* Test 11 copy over with buffer flush after each read write, buffer size 1*/
    test_flush(src_filename1, dst_filename1, dst_filename2, 1);
    printf("Flush after every write, copy file sequence: buffer size 1\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }

    /* Test 12 simple copy over from one file to another with a null byte in the middle*/
    simple_copy_sequence(src_filename3, dst_filename1, dst_filename2, 300);
    printf("Simple copy file sequence: null byte in middle of src file\n");
    compare_files(dst_filename1, dst_filename2);
    if ((remove(dst_filename1) | remove(dst_filename2)) != 0){
        perror("remove");
    }
}

void test_flush(char * src_filename, char * dst_filename1, char * dst_filename2, int bytes_per_iter){
    int bytes_read;
    char ptr[bytes_per_iter];
    struct file *in, *out;
    char buf[bytes_per_iter];
    FILE *src, *dst;

    if((in = myopen(src_filename, O_RDONLY )) == NULL) {
        perror("myopen");
        exit(3);
    }

    if((out = myopen(dst_filename1, O_TRUNC | O_CREAT | O_WRONLY)) == NULL) {
        perror("myopen");
        exit(3);
    }
    while((bytes_read = myread(in, ptr, bytes_per_iter)) > 0) {
        mywrite(out, ptr, bytes_read);
        myflush(out);
    }
    if(myclose(in) != 0) {
        perror("myclose");
        exit(1);
    }
    if(myclose(out) != 0) {
        perror("myclose");
        exit(1);
    }

    
    src = fopen(src_filename, "r");
    dst = fopen(dst_filename2, "w");

    while((bytes_read = fread(buf, 1, bytes_per_iter, src)) > 0) {
        fwrite(buf, 1, bytes_read, dst);
        fflush(dst);
    }
    fclose(dst);
    fclose(src);
}


void simple_copy_sequence (char * src_filename, char * dst_filename1, char * dst_filename2, int bytes_per_iter){
    int bytes_read, bytes_written;
    char ptr[bytes_per_iter];
    struct file *in, *out;

    if((in = myopen(src_filename, O_RDONLY )) == NULL) {
        perror("myopen");
        exit(3);
    }

    if((out = myopen(dst_filename1, O_TRUNC | O_CREAT | O_WRONLY )) == NULL) {
        perror("myopen");
        exit(3);
    }

    while((bytes_read = myread(in, ptr, bytes_per_iter)) > 0) {
        bytes_written = mywrite(out, ptr, bytes_read);
        if(bytes_written < 0) {
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

    char buf[bytes_per_iter];
    int src_fd, dst_fd;

    if((src_fd = open(src_filename, O_RDONLY)) < 1) {
        perror("open");
        exit(2);
    }

    if((dst_fd = open(dst_filename2, O_TRUNC | O_CREAT | O_WRONLY, 0666)) < 0) {
        perror("open");
        exit(3);
    }

    while((bytes_read = read(src_fd, buf, bytes_per_iter)) > 0) {
        bytes_written = write(dst_fd, buf, bytes_read);
        if(bytes_written < 0) {
            perror("write");
            exit(4);
        }
    }

    if(close(src_fd) != 0) {
        perror("close");
        exit(1);
    }
    if(close(dst_fd) != 0) {
        perror("close");
        exit(1);
    }
}


void seek_copy_write(char * src_filename, char * dst_filename1, char * dst_filename2, int bytes_per_iter, int offset, int whence){
    int bytes_read, bytes_written;
    char ptr[bytes_per_iter];
    struct file *in, *out;

    if((in = myopen(src_filename, O_RDONLY )) == NULL) {
        perror("myopen");
        exit(3);
    }

    if((out = myopen(dst_filename1, O_TRUNC | O_CREAT | O_WRONLY )) == NULL) {
        perror("myopen");
        exit(3);
    }

    myseek(out, offset, whence);
    while((bytes_read = myread(in, ptr, bytes_per_iter)) > 0) {
        bytes_written = mywrite(out, ptr, bytes_read);
        if(bytes_written < 0) {
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

    char buf[bytes_per_iter];
    int src_fd, dst_fd;

    if((src_fd = open(src_filename, O_RDONLY)) < 1) {
        perror("open");
        exit(2);
    }

    if((dst_fd = open(dst_filename2, O_TRUNC | O_CREAT | O_WRONLY, 0666)) < 0) {
        perror("open");
        exit(3);
    }

    lseek(dst_fd, offset, whence);
    while((bytes_read = read(src_fd, buf, bytes_per_iter)) > 0) {
        bytes_written = write(dst_fd, buf, bytes_read);
        if(bytes_written < 0) {
            perror("write");
            exit(4);
        }
    }

    if(close(src_fd) != 0) {
        perror("close");
        exit(1);
    }
    if(close(dst_fd) != 0) {
        perror("close");
        exit(1);
    }
}


void seek_copy_read(char * src_filename, char * dst_filename1, char * dst_filename2, int bytes_per_iter, int offset, int whence){
    int bytes_read, bytes_written;
    char ptr[bytes_per_iter];
    struct file *in, *out;

    if((in = myopen(src_filename, O_RDONLY )) == NULL) {
        perror("myopen");
        exit(3);
    }

    if((out = myopen(dst_filename1, O_TRUNC | O_CREAT | O_WRONLY )) == NULL) {
        perror("myopen");
        exit(3);
    }

    myseek(in, offset, whence);
    while((bytes_read = myread(in, ptr, bytes_per_iter)) > 0) {
        bytes_written = mywrite(out, ptr, bytes_read);
        if(bytes_written < 0) {
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

    char buf[bytes_per_iter];
    int src_fd, dst_fd;

    if((src_fd = open(src_filename, O_RDONLY)) < 1) {
        perror("open");
        exit(2);
    }

    if((dst_fd = open(dst_filename2, O_TRUNC | O_CREAT | O_WRONLY, 0666)) < 0) {
        perror("open");
        exit(3);
    }

    lseek(src_fd, offset, whence);
    while((bytes_read = read(src_fd, buf, bytes_per_iter)) > 0) {
        bytes_written = write(dst_fd, buf, bytes_read);
        if(bytes_written < 0) {
            perror("write");
            exit(4);
        }
    }

    if(close(src_fd) != 0) {
        perror("close");
        exit(1);
    }
    if(close(dst_fd) != 0) {
        perror("close");
        exit(1);
    }
}

void compare_files(char *file_path1, char *file_path2){
    FILE *fp1 = fopen(file_path1, "r");
    FILE *fp2 = fopen(file_path2, "r");
  
    if (fp1 == NULL || fp2 == NULL)
    {
       printf("Error : Files not open");
       exit(0);
    }

    char ch1 = getc(fp1);
    char ch2 = getc(fp2);
  
    int error = 0, pos = 0, line = 1;
  
    while (ch1 != EOF && ch2 != EOF)
    {
        pos++;
        if (ch1 == '\n' && ch2 == '\n')
        {
            line++;
            pos = 0;
        }
        if (ch1 != ch2)
        {
            error++;
            printf("Line Number : %d \tError"
               " Position : %d \n", line, pos);
        }
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    }

    fclose(fp1);
    fclose(fp2);

    if (error == 0){
        printf("\t\033[0;32mTEST PASSED\033[0m: output equal\n");
    }
    else{
        printf("\t\033[0;33mTEST FAILED\033[0m: output different\n");
    }
}