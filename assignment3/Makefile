CFLAGS=-Wall -pedantic -g

# test-my-malloc: test-my-malloc.c my-malloc.so
# 	gcc $(CFLAGS) -o test-my-malloc test-my-malloc.c

my-malloc: my-malloc.c
	gcc $(CFLAGS) -o my-malloc my-malloc.c

my-malloc.so: my-malloc.c
	gcc $(CFLAGS) -rdynamic -shared -fPIC -o my-malloc.so my-malloc.c

.PHONY: clean
clean:
	rm -f my-malloc test-my-malloc *.o *.so
