CFLAGS=-Wall -pedantic -g

test-myio: test-myio.o libmymmapio.so
	gcc -L. -lmymmapio -o test-myio test-myio.o

test-myio.o: test-myio.c
	gcc $(CFLAGS) -c -o test-myio.o test-myio.c

libmymmapio.so: mymmapio.o
	gcc -shared -Wl,-soname,libmymmapio.so -o libmymmapio.so mymmapio.o

mymmapio.o: mymmapio.c
	gcc $(CFLAGS) -fPIC -c -o mymmapio.o mymmapio.c

# myio: mymmapio.c
# 	gcc $(CFLAGS) -o myio mymmapio.c

# playground: playground.c
# 	gcc $(CFLAGS) -o playground playground.c

.PHONY: clean
clean:
	rm -f myio test_files/test_to_* *.o test-myio *.so playground