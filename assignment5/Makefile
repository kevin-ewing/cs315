CFLAGS=-Wall -pedantic -g

mysh: mysh.c
	gcc $(CFLAGS) -o mysh mysh.c

mysh2: mysh2.c
	gcc $(CFLAGS) -o mysh2 mysh2.c

playground: playground.c
	gcc $(CFLAGS) -o playground playground.c

.PHONY: clean
clean:
	rm -f mysh playground