CFLAGS=-Wall -pedantic -g

# test_myio: test_myio.o myio.o
# 	gcc -o test_myio test_myio.o myio.o

# test_myio.o: test_myio.c
# 	gcc $(CFLAGS) -c -o test_myio.o test_myio.c

# myio.o: myio.c
# 	gcc $(CFLAGS) -c -o myio.o myio.c

myio: myio.c
	gcc -g -o myio myio.c

.PHONY: clean
clean:
	rm -f myio test_files/test_to.txt *.o test_myio

clean_out:
	rm -f test_files/test_to.txt