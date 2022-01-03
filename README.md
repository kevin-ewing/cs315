# CS315
Project work from CS315

## Assignment 01
If any of this is unclear, please get in touch with me ASAP. There are definitely things in the course I want you to struggle with. I absolutely do NOT want you struggling with understanding my expectations.

In this assignment, you will implement basic functionality of the ls program. Specifically, your implementation must work the same as the real ls program under the following scenarios:

when run with no arguments;
and when run with any combination of the arguments -l, -a, any number of files, and any number of directories ("any number" within reason).
You will have to experiment and/or research how ls behaves in these cases so that you can duplicate it.

The logic itself is not terribly complicated; I expect and intend much of your time will be spent reading man pages and acclimating yourself to maneuvering around Linux and C.

Goals
The goals of this assignment are for you to

begin developing proficiency with C;
begin developing familiarity with man pages;
design and implement a fairly basic program.
New Mechanics
You might find these useful in this assignment.

New functions
`opendir(3)`
`readdir(3)`
`closedir(3)`
`getopt(3)`
`stat(3p)`
`strftime(3)`
`getpwnam(3)`
`localtime(3)`
Separately, the `stat(2)` manpage lists the fields of the struct stat populated by `stat(3p)`; the `inode(7)` manpage goes into further detail. Note that file permissions (eg, read, write, execute) are called the "mode".

### Behavioral requirements
You will produce a source file, `myls.c`, that implements the required logic. When compiled and run with no arguments, it should list all non-hidden files in the current directory. When supplied with a list of arguments, it should behave as ls does: for all non-directories, list the files themselves; for all directories, list the contents; error if a file doesn’t exist. Ordering of the output is at your discretion (you will notice that, in a listing with both files and directories, ls will show all files first and then all directories, regardless of the order they appeared on the command-line; you do not need to do this). Unless a single filename exceeds 80 characters, the output of your program should not exceed 80 columns.

Furthermore, your ls program must accept the -a and -l options. When provided with the -a option, your program should list all files in the directories given, including hidden files. When provided with the -l option, your program should provide detailed information about each file, one file per line, just as the standard ls program does.

When provided with the -l option, your implementation does not need to print the "total ##" line that the standard version of ls prints.

You must use `getopt(3)` to parse command-line arguments.

You do not need to match the spacing of the real ls.

### Deliverable requirements
`myls.c`, which should contain your implementation of ls.

`Makefile`, whose default target should build `myls` (out of `myls.c`), It may have as many other targets as you like, to make your life easier), but the default one must build that program. It must also have a clean target that removes all built files.

`README`, which will include a list of authors, a list of known bugs, and a list of resources you consulted in your work (a list of URLs is totally acceptable—if I can see the kinds of references you’re using, I can get a better idea of how to tailor the instruction to your needs—just keep the README file open as you work and paste them there when you find something useful).

### Style requirements
Your code must exhibit:

appropriate use of comments;
appropriate use of stack, heap, global, and static variables;
appropriate use of constants;
no magic numbers;
appropriate decomposition (a good rule of thumb is functions should be no more than 1 screen long);
clear organization (eg, grouped header files, struct definitions, function prototypes);
well-named variables, functions, and other identifiers;
verification of command-line arguments, if applicable;
checking and appropriate handling of all return values;
consistent style (capitalization, indenting, etc).


## Assignment 02
If any of this is unclear, please get in touch with me ASAP. There are definitely things in the course I want you to struggle with. I absolutely do NOT want you struggling with understanding my expectations.

As discussed in class, using system calls directly can be horrifically inefficient. Specifically with regards to the I/O-related system calls, consider an imaginary program that produces 1 byte of output at a time, and does this continually throughout its lifetime. Calling write(2) every time it produces a single byte is going to be wasteful; it makes far more sense to save up those individual bytes of output and to only call write(2) once it has accumulated "enough" (to be defined later).

Saving intermediate results like this is called buffering and, not at all coincidentally, an arbitrary chunk of memory used for storing stuff is often called a buffer. This is directly related to the dreaded "Buffering…" message one sees from Netflix: your computer has a chunk of memory (a buffer) in which it stores the next few seconds of video. Your computer is simultaneously reading from that buffer to display the video and also receiving new data from Netflix into the buffer. Your video stops playing when the buffer is empty; the message indicates that Netflix has to send more data to catch up with the rate at which the buffer is emptied.

For this assignment, you will produce a collection of functions that insulate a program from the inefficiencies of I/O-related system calls using the method discussed in class. You will create functions analogous to the system calls open(2), close(2), read(2), write(2), and lseek(2). You will also implement a function to flush a buffer: that is, to force all buffered data to be delivered to its destination (note that this only makes sense in the context of writing).

As implied above, the question of what constitutes "enough" is important. Traditionally, there are three options: unbuffered, in which data is immediately delievered to/from a process to the corresponding system call; line-buffered, in which the abstraction layer buffers a single line (ie, a string that ends in a newline character); and block buffered, in which the buffer is a fixed size (a "block"). For this assignment, you will implement a block-buffering scheme, whose block size is configurable at compile time.

In addition to the system calls enumerated above, you will likely find these library functions useful in your implementation:

`memcpy(3)`
`malloc(3)`
`free(3)`
Goals
Abstractly, the goals of this assignment are for you to:

internalize the potential inefficiency of system calls;
refresh your facility with pointers and the heap;
expand your use of Makefiles;
continue to expand your experience with C and design/implementation of non-trivial programming projects.


### Behavioral requirements
You will produce one source file, `myio.c`, that contains implementations of the following functions with the described behavior:

`myopen`, analogous to `open(2)`. You must support these flags: O_CREAT, O_WRONLY, O_RDONLY, O_RDWR, O_TRUNC. In the case of O_CREAT and O_TRUNC, you may assume a mode of 0666 in all cases.
`myclose`, analogous to `close(2)`.
`myread`, analogous to `read(2)`.
`mywrite`, analogous to `write(2)`.
`myseek`, analogous to `lseek(2)`. We have not discussed this one, so read the manpage to understand its behavior. You need only implement the SEEK_SET and SEEK_CUR options.
`myflush`, which forces any buffered data to its destination (eg, if the buffer has 12 bytes in it and is waiting for another 37 before it actually calls `write(2)`, a call to myflush will immediately cause those 12 bytes to be written, regardless of buffer occupancy).
Note that your my* functions will themselves issue system calls. For example, the only way to open a file for writing is to issue the `open(2)` system call, so presumably your myopen will (among other things) call open(2). For further example, the program may call mywrite a bunch of times but the bytes being "written" will be saved up and only when enough have accumulated will mywrite actually call write(2) to push them to disk. This aspect is particularly important, so please talk to me if the desired behavior is not clear.

The various manpages imply but do not explicitly state one important fact: the same file offset is used for both reading and writing. (Thus this is only applicable when a file is opened with the O_RDWR flag.) So if I open the file and then read 12 bytes (ie, bytes 0 through 11), the next operation (either reading or writing) will start at byte 12. In the case of a write operation, the bytes starting with the 12th will be overwritten.

Your implementation must allow a program to have an arbitrary number of files open simultaneously.

The size of the buffer must be easily configurable at compile time.

Testing
You will also produce any number of source files that test the behavior of the aforementioned functions. You might have one program to test simple reading, another to test simple writing, and others to test more complicated operations. The requirements for these programs are that the precise behavior they are testing must be documented.

You will find that these programs do not compile cleanly without function prototypes for all the my* functions. To solve this, create a header file called myio.h that contains those prototypes. Then, in the programs, you need only #include <myio.h>. (We will discuss header files in more detail when we get deeper into libraries, but for now this will do.)

### Deliverable requirements
At least five files:
`myio.c` and `myio.h`, whose contents and behavior are described above.
At least one C program that tests the behavior of `myio.c`, the name(s) of which is/are at your discretion.
`Makefile`, whose default target should build all the test programs. It must also have a clean target to remove all generated files. You may have additional targets if you like.
(Optional) A shell script that executes all your tests.
`README`, which will include a list of authors, a list of known bugs, and a list of resources you consulted in your work (a list of URLs is totally acceptable—if I can see the kinds of references you’re using, I can get a better idea of how to tailor the instruction to your needs—just keep the README file open as you work and paste them there when you find something useful). The README file should also describe how to test your library.

### Style requirements
Your code must exhibit:

appropriate use of comments;
appropriate use of stack, heap, and global variables;
appropriate use of constants;
appropriate decomposition;
clear organization (eg, grouped header files, struct definitions, function prototypes);
well-named variables, functions, and other identifiers;
verification of command-line arguments, if applicable;
checking and appropriate handling of all return values;
consistent style (capitalization, indenting, etc).








## Assignment 03
If any of this is unclear, please get in touch with me ASAP. There are definitely things in the course I want you to struggle with. I absolutely do NOT want you struggling with understanding my expectations.

In this assignment, you will implement replacements for malloc(3) and free(3), along with variants of the former, calloc(3) and realloc(3), and a related function, malloc_usable_memory(3). As discussed in class, for efficiency reasons, these are not system calls; instead, they use the brk(2) and sbrk(2) system calls to request large chunks of memory from the kernel, out of which malloc(3) itself satisfies the individual requests. (In this respect, it’s more than a little similar to myread.) On the flip-side, free(3) reclaims chunks of memory that are no longer in use.

You will create a dynamically-linkable library that contains your implementations, and use the LD_PRELOAD mechanism (use described below) to cause it to be loaded and linked before libc, which contains the official implementations of malloc(3) and free(3). By linking it before libc, the implementations in your library will be the ones whose addresses get put in the global offset table, and hence whenever any program run in that manner calls malloc, it will get your implementation and not the offical one.

This presents an interesting problem: functions like printf(3) use malloc(3) to do their thing and, during development, if your implementation isn’t yet correct, then printf(3) isn’t going to work! Two complementary suggestions: a) use write(2) instead (recall that the file descriptor corresponding to stdout is 1); 2) make extensive use of gdb. (How to use gdb with LD_PRELOAD is described below.)

Without printf(3), though, we lose the ability to format pointers and integers as human-readable text (recall the difference between how numerical values are represented in memory and the ASCII characters that humans use to interpret such a value). I have written a function to help you with this; both the function and driver code are available here: pointer_to_hex_le.c. I have also written a similar function (along with driver code) for unsigned integers: uint64_to_string.c.

Your implementations need not be terribly clever, but they do need to work, even when real programs like ls and cat use them. I am being intentionally vague about what it means to "work" because I want you to think about your use of malloc(3) in the past, the behavior of the relevant system calls, and how you can unify these ideas.

NOTE: The pointers returned by your implementation of malloc must be evenly divisible by 16. (And it is probably a good idea if the bookkeeping struct is also on a 16-byte boundary.)

For context, curiosity, and light bedtime reading, you might be interested in how the real thing works. There are actually many different implementations running around; some of the biggies (at least in the UNIX world) are/were:

Doug Lea’s malloc ("dlmalloc"), probably best described in this Phrack paper;
jemalloc, originally presented in this paper; and
ptmalloc, currently used in glibc, and for which I could not find a good description (the typical response being, "Use the Source, Luke!").
New Mechanics
You might find these useful in this assignment.

New system calls/functions
`brk(2)`
`sbrk(2)`
`memset(3)`
Language features
You might be tempted to declare a global variable in your library. Under normal circumstances, such a variable would become a symbol that can be linked by a process that loads your library (ie, it is exported). This might be harmful if the name of your variable happens to match the name of another variable already in the process. To get around this, you can precede the declaration of your variable with the static keyword, which restricts the symbol to the file in which it is declared—ie, it is not exported.

Building, running, and debugging
You need a few extra arguments to gcc to create a shared library. Therefore, to compile my-malloc.c as a library called my-malloc.so that can be linked at runtime, use a command line this:

`$ gcc -g -Wall -pedantic -rdynamic -shared -fPIC -o my-malloc.so my-malloc.c`
If that my-malloc.so library defines functions like malloc and free (just to pick two completely random examples), you can cause your implementations to override the libc ones in a single process, start said process with a command like this:

`$ LD_PRELOAD=./my-malloc.so ls`
As I said in class, you will likely find gdb really helpful for this assignment. If you use the LD_PRELOAD trick above with gdb, however, anytime gdb wants to use malloc, it will use your implementation, which might be unhelpful if your implementation is buggy. Therefore, to cause gdb to use the special library only for the program to be debugged, use a command like this:

`$ gdb --args env LD_PRELOAD=./my-malloc.so ./test-malloc`
If you find yourself using these commands over and over again, you might want to figure out ways to make them more convenient to run—more convenient even than pressing the up-arrow through your shell history. (Hint: Makefile.)

### Behavioral requirements
Your library must implement the functions malloc, free, calloc, realloc, and malloc_usable_size with the behavior described in the relevant manpages. When this library is pre-loaded, modest programs like ls and cat must work correctly. I won’t test your library against ambitious programs like Firefox. (For the record, my implementation works with vim, though, and it’s pretty usable.)

Your library may not just ask sbrk(2) for a bazillion bytes once at the beginning and then allocate out of that. It must be moderately intelligent about growing the size of the heap as necessary.

Your library should, where possible, reuse chunks of memory that have been relinquished using free.

Addresses returned by your implementation of malloc, calloc, and realloc must be 16-byte aligned.

### Deliverable requirements
Three files:

`my-malloc.c`, which implements the functions described above.

`Makefile`, whose default target should build the aforementioned source file into a shared library named my-malloc.so. It may have as many other targets as you like (and probably should, to make your life easier), but the default one must build the library my-malloc.so.

`README`, which will include a list of authors, a list of known bugs, and a list of resources you consulted in your work (a list of URLs is totally acceptable—if I can see the kinds of references you’re using, I can get a better idea of how to tailor the instruction to your needs—just keep the README file open as you work and paste them there when you find something useful).

### Style requirements
Your code must exhibit:

appropriate use of comments;
appropriate use of stack, heap, global, and static variables;
appropriate use of constants;
appropriate decomposition;
clear organization (eg, grouped header files, struct definitions, function prototypes);
well-named variables, functions, and other identifiers;
verification of command-line arguments, if applicable;
checking and appropriate handling of all return values;
consistent style (capitalization, indenting, etc).







## Assignment 04
If any of this is unclear, please get in touch with me ASAP. There are definitely things in the course I want you to struggle with. I absolutely do NOT want you struggling with understanding my expectations.

In this assignment, you will implement your own runtime linker. The ultimate result will be a program that, when run, will call a function in libc that does not cause the official linker to run. You will presumably precede this function call by code of your own devising that will obviate the need to call the official linker. You may hardcode memory addresses as necessary and able but, when you do, you must document the hardcoded address with its source: describe in great detail how you decided on each particular address.

Additionally, you must turn in the script of a gdb session in which you demonstrate that your runtime linker does what it should do. Annotate this script with notes that make perfectly clear why it does indeed do what it’s supposed to do. Unless you want to copy and paste a lot, you might find the script(1) program handy here.

You will find the following functions useful:

`dlopen(3)`
`dlclose(3)`
`dlsym(3)`
Your program must work both from the shell and from within gdb.

Casting
You will also likely want to perform casting, which is a note to the compiler to ignore type mismatches. Consider the following code:
`
char *c;
int *i;

c = i;
`
The compiler will reject this code because the types of the variable on the left-hand side of the assignment (c, a character pointer) does not match the type of the value on the right-hand side of the assignment (i, an integer pointer). Assuming you are 100% sure you know what you’re doing, you can get around this by telling the compiler the type of an expression, as in the following code, which will compile.

`c = (char *) i;`
Global offset table
You will need to locate the global offset table (GOT) in memory. Fortunately, there exists a symbol that will help! To use it, you must declare it as an extern variable (because it is technically an undefined reference, even though you are going to be using it within your program). Note that this variable is the first entry; it does not point to the first entry. If you want its address, you’ll have to ampersand it.

`extern void *_GLOBAL_OFFSET_TABLE_;

printf("The first entry of the global offset table is %p\n", _GLOBAL_OFFSET_TABLE_);
printf("The GOT starts at address %p\n", &_GLOBAL_OFFSET_TABLE_);`

### Behavioral requirements
Your program must issue a call to an external function (something in libc, like puts or strcpy is easiest). The GOT entry for this external function must point to the actual implementation of the function before the function is ever called from your program. This means you must implement logic that executes before the function call in question that modifies the GOT appropriately.

Should you hard-code memory addresses, you must document exactly how you calculated them. It’s easiest if you do so in comments near the use, rather than in the README.

Your program must work from the shell as well as within gdb.

### Deliverable requirements
Three files:

`self-linker.c`, whose contents and behavior are described above.

`Makefile`, whose default target should build the aforementioned source file into a program named self-linker. You may have other targets if you like, but the default one must build self-linker.

`README`, which will include a list of authors, a list of known bugs, and a list of resources you consulted in your work (a list of URLs is totally acceptable—if I can see the kinds of references you’re using, I can get a better idea of how to tailor the instruction to your needs—just keep the README file open as you work and paste them there when you find something useful).

The README should also contain your annotated gdb session that demonstrates the correctness of your implementation.

### Style requirements
Your code must exhibit:

appropriate use of comments;
appropriate use of stack, heap, and global variables;
appropriate use of constants;
appropriate decomposition;
clear organization (eg, grouped header files, struct definitions, function prototypes);
well-named variables, functions, and other identifiers;
verification of command-line arguments, if applicable;
checking and appropriate handling of all return values;
consistent style (capitalization, indenting, etc).
Submission Process







## Assignment 05
If any of this is unclear, please get in touch with me ASAP. There are definitely things in the course I want you to struggle with. I absolutely do NOT want you struggling with understanding my expectations.

In this assignment, you will write your own shell. It must print a prompt, accept commands from the user, support input/output redirection, and pipelines. A description of these behaviors follows.

Input redirection
Normally, a program accepts input from stdin, which is typically attached to the terminal in which the program is running. A program can be made to read from a file instead using the following syntax.

`$ ./foo < input-file`
Note that the program itself does not know the difference. It just reads from file descriptor 0; the shell just does some tricks behind the scenes to make sure file descriptor 0 is associated with input-file instead of the terminal.

Output redirection
Normally, a program’s output goes to stdout, which is typically the terminal in which the program is running. That output can be caused to go to a file instead using the following syntax.

`$ ./bar > output-file`
As with input redirection, the program doesn’t know this is happening: it just writes to file descriptor 1, which the shell has caused to be associated with output-file instead of the terminal.

In the previous invocation method, the file output-file should be truncated (made zero-length) before any writing begins. An alternate invocation method causes any data output by the program to be appended to the file instead:

`$ ./baz >> output-file2`
Piping
Instead of saving the output in a file, the shell can cause a program’s output to be delivered directly to another program’s input. This technique is called piping and is indicated using the pipe character: "|".

`$ ./program1 | ./program2`
In this instance, program1 happily writes to file descriptor 1 and program2 happily reads from file descriptor 0. Except the shell has done some magic to ensure that whatever program1 writes to its fd1 appears on fd0 of program2.

With two exceptions, the net effect is equivalent to the following command.

`$ ./program1 > temp
$ ./program2 < temp`
The two exceptions are that, in the case of piping, the programs are running simultaneously, whereas in the latter example they are running sequentially; and the latter example creates a file to temporarily store the output before passing it on to the second program, whereas with piping there is no temporary file.

New Mechanics
You might find these useful in this assignment.

New functions
`fork(2)`
`exec(3)`
`wait(2)`
`pipe(2)`
`dup(2)`
`fgets(3)`
`strtok(3)`
Some of these are not strictly necessary (ie, you can implement the assignment without them) but they are sufficiently standard for the types of problems you’re going to be solving herein that you should be aware of their existence.

### Behavioral requirements
You will produce a source file, mysh.c, that implements your shell. When compiled and run, it should behave just like the shell you interact with:

it should print a prompt (it doesn’t have to be fancy, but it should be clear when the shell is awaiting a command);

when a command is typed (ending in newline), it should run the command;

when the command has finished, it should print a new prompt.

It should continue reading and running commands until it reads end-of-file or the user enters the command "exit". (From the keyboard, you can indicate end-of-file by hitting Ctrl-D.)

If the command contains input redirection (<), output redirection (> and >>), or pipes (|), it should behave as described above.

In the case of piping, your shell should support an arbitrary number of pipes in a single command.

You may make some assumptions about commands being inputted:

A single input will never exceed 4096 characters in length (including the trailing newline and null terminator).

Individual commands (ie, single entities for which the shell will fork(2), and which may be separated by pipes) will have a maximum of 10 arguments, arguments will be separated by a single space, and commands will features neither leading nor trailing spaces. (I don’t want you to get too hung up on parsing.)

Any I/O redirection or piping will be correctly specified and not contradictory (eg, I won’t enter a command that uses both ">" and "|" for the same program’s output).

Things you don’t need to worry about:

the environment.
(I may add clarifications here as questions arise.)

### Deliverable requirements
`mysh.c`, which should contain the implementation of your shell.

`Makefile`, whose default target should build mysh (out of mysh.c), It may have as many other targets as you like, to make your life easier), but the default one must build that program.

`README`, which will include a list of authors, a list of known bugs, and a list of resources you consulted in your work (a list of URLs is totally acceptable—if I can see the kinds of references you’re using, I can get a better idea of how to tailor the instruction to your needs—just keep the README file open as you work and paste them there when you find something useful).

### Style requirements
Your code must exhibit:

appropriate use of comments;
appropriate use of stack, heap, global, and static variables;
appropriate use of constants;
appropriate decomposition;
clear organization (eg, grouped header files, struct definitions, function prototypes);
well-named variables, functions, and other identifiers;
verification of command-line arguments, if applicable;
checking and appropriate handling of all return values;
consistent style (capitalization, indenting, etc).


## Assignment 06
If any of this is unclear, please get in touch with me ASAP. There are definitely things in the course I want you to struggle with. I absolutely do NOT want you struggling with understanding my expectations.

In this assignment, you will implement the six functions from Assignment 2 using mmap(2) wherever possible. (Use the same function and struct names—and even the same header file.) The result will be a shared library, which will be linked at runtime with your test program.

You need not implement any buffering: mmap itself takes care of loading the data into memory as necessary (the precise mechanics will be discussed when we talk about virtual memory and/or you take Operating Systems).

New Mechanics
You might find these useful in this assignment.

New functions
`truncate(2)`
Compiling a shared library
You compiled a shared library in Assignment 3, but that was a slightly special case because it was intended specifically for use with LD_PRELOAD. Compiling a "real" shared library requires some different steps.

First, you must compile the constituent source files using the `-fPIC` flag. Then, when you have compiled the source file into object code, use this command to make a shared library out of it (ie, this is the linking step):

`$ gcc -shared -Wl,-soname,libmymmapio.so -o libmymmapio.so mymmapio.o`
Linking with a shared library
Normally, the linker will look in a set of preconfigured directories to find libraries against which to link. The current directory (which is presumably where, eg, libmyio.so will reside) is not in this list. When linking, use the -L flag to tell gcc where else to look for libraries. Therefore, to compile foo.o into an executable named foo, and to link with libbar.so which is in the current directory ("."), use this command:

`$ gcc -L. -lbar -o foo foo.o`
Note that this will work even if the library has a version number attached to it. So if you want to link with libbar.so.4.3.2, you still use -lbar and it will find the correct file.

Running with a shared library (in a non-traditional location)
You linked to the library using the preceding command, and now you go to run it… but how does the program know where to find the library? The "-L." parameter during compilation doesn’t tell the program where to find the library when it’s actually being run. And, really, there’s no way it can: what if the library is moved? What if the program itself is moved and "." suddenly refers to a completely different directory?

As with compile-time linking, there are a preconfigured set of directories searched for libraries to be linked at runtime [1]. The LD_LIBRARY_PATH environment variable augments this list. Thus, to cause the runtime linker to look in the current directory (in addition to default directories) for libraries, run the program like so:

`$ LD_LIBRARY_PATH=. ./foo`

### Behavioral requirements
You will produce a source file, test-myio.c, that contains a set of tests to verify the correctness of your I/O libraries. As discussed in class, one way to test whether your my* functions work correctly is to call the exact same sequence of non-my functions and compare the results. The key is going to be choosing the sequence of function calls that adequately test your code. You should probably use our discussion of basic blocks to guide your thought process in devising these tests.

I should be able to link this program against either of your I/O libraries to test them.

When run, the test program should print a short description of each test and whether or not it passed. Your tests should test all functionality of your libraries. If you would like to discuss your tests to make sure they are sufficient, I’m happy to do so.

### Deliverable requirements
Lots of files:

`mymmapio.c`, which should contain the functions implemented using mmap(2) where possible.

`myio.h`, a header file corresponding to the preceding source file.

`test-myio.c`, which should contain the testing code described above.

`Makefile`, whose default target should build libmymmapio.so (out of `mymmapio.c`), and `test-myio`. It may have as many other targets as you like (and probably should, to make your life easier), but the default one must build those files. Your Makefile should have separate recipes for compiling and linking.

`README`, which will include a list of authors, a list of known bugs, and a list of resources you consulted in your work (a list of URLs is totally acceptable—if I can see the kinds of references you’re using, I can get a better idea of how to tailor the instruction to your needs—just keep the README file open as you work and paste them there when you find something useful).

### Style requirements
Your code must exhibit:

appropriate use of comments;
appropriate use of stack, heap, global, and static variables;
appropriate use of constants;
appropriate decomposition;
clear organization (eg, grouped header files, struct definitions, function prototypes);
well-named variables, functions, and other identifiers;
verification of command-line arguments, if applicable;
checking and appropriate handling of all return values;
consistent style (capitalization, indenting, etc).





## Assignment 07
If any of this is unclear, please get in touch with me ASAP. There are definitely things in the course I want you to struggle with. I absolutely do NOT want you struggling to understand my expectations.

In this assignment, you will implement both server and client for a simple online chat system. Every client will represent a user, which may have a name associated with it. Upon connecting, the name will be "unknown", but a user may change their name by sending the message "/nick <new nickname>" to the server. The server will announce the name change to all connected clients. Similarly, any message sent by a client will appear for all clients.

Following are example runs of a single server and two separate clients running concurrently.

Server (all text output by server process, pressed Ctrl-C after first client exited):

`$ ./chat-server 8000
new connection from 127.0.0.1:46814
new connection from 127.0.0.1:46816
User unknown (127.0.0.1:46814) is now known as foo.
User unknown (127.0.0.1:46816) is now known as bar.
Lost connection from foo.
^C`
Client 1 (the lines consisting of only "/nick foo", "hi", and "I’m just fine, thanks" were input at the terminal, followed by Ctrl-D):

`$ ./chat-client localhost 8000
Connected
/nick foo
22:31:37: User unknown (127.0.0.1:46814) is now known as foo.
22:31:41: User unknown (127.0.0.1:46816) is now known as bar.
hi
22:33:40: foo: hi
22:33:42: bar: hello!
22:33:45: bar: how are you?
I'm just fine, thanks
22:33:48: foo: I'm just fine, thanks
Exiting.`
Client 2 (the lines consisting of only "/nick bar", "hello!", and "how are you?" were input at the terminal, exited when server terminated):

`$ ./chat-client localhost 8000
Connected
22:31:37: User unknown (127.0.0.1:46814) is now known as foo.
/nick bar
22:31:41: User unknown (127.0.0.1:46816) is now known as bar.
22:33:40: foo: hi
hello!
22:33:42: bar: hello!
how are you?
22:33:45: bar: how are you?
22:33:48: foo: I'm just fine, thanks
22:34:01: User foo (127.0.0.1:46814) has disconnected.
Connection closed by remote host.`
New Mechanics
You might find these useful in this assignment.

New(ish) functions
`kill(2)`
`time(2)`
`strncpy(3)`
`snprintf(3)`
(In addition to those functions introduced in the various course notes.)

New machines
You have used basin.cs.middlebury.edu already in this course to store your upstream git repos. Unfortunately, as we discovered in class, the firewall on basin does not allow arbitrary incoming network connections, and so you won’t be able to use it to test your chat server. The good news is that there is another machine available for this purpose: you may ssh to weathertop.cs.middlebury.edu and run your servers there. weathertop accepts incoming connections on ports 4000 through 4020, so use a port in that range. Recall that multiple sockets can’t bind to the same port; you can use the command netstat -tnl to see which ports are in use. If you are off-campus, you will need the VPN active to connect to weathertop, just as you do for basin.

### Behavioral requirements
For the server…

Its single command-line argument should be the port number upon which to listen for incoming connections.
Every client must be handled by a separate thread.
When a client connects, the server should print a message to stdout identifying the new client by IP address and port number.
When a client changes its nickname, the server should print a message to stdout indicating both old and new names.
When a client disconnects, the server should print a message a message to stdout indicating so.
Must support an arbitrary number of clients connected simultaneously.
For the client…

Takes two command-line arguments: the hostname and port number of the server.
Upon receiving a message from the server, prints it out preceded by a timestamp.
It is acceptable if the terminal becomes garbled when a message arrives while the user in the midst of typing their own message.
If the user enters end-of-file (ie, the user types Ctrl-D), the client should terminate with a descriptive message.
If the connection is terminated (eg, the server process terminates), the client should also terminate, with a descriptive message.
For the system…

When a client sends a message, every client connected to the server should receive it and print it out, with proper attribution.
When a client disconnects from the server (for whatever reason), the server should notify all remaining clients, which will print an informative message.
### Deliverable requirements
`chat-server.c` and `chat-client.c`, which contain the code for the server and client, respectively.

`Makefile`, whose default target should build chat-server (out of chat-server.c) and chat-client (out of chat-client.c). It may have as many other targets as you like, to make your life easier), but the default one must build those programs.

`README`, which will include a list of authors, a list of known bugs, and a list of resources you consulted in your work (a list of URLs is totally acceptable—if I can see the kinds of references you’re using, I can get a better idea of how to tailor the instruction to your needs—just keep the README file open as you work and paste them there when you find something useful).

### Style requirements
Your code must exhibit:

appropriate use of comments;
appropriate use of stack, heap, global, and static variables;
appropriate use of constants;
appropriate decomposition;
clear organization (eg, grouped header files, struct definitions, function prototypes);
well-named variables, functions, and other identifiers;
verification of command-line arguments, if applicable;
checking and appropriate handling of all return values;
consistent style (capitalization, indenting, etc).
