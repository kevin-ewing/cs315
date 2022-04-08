/*
* mysh2.c
*
* First attempt at implementation of a shell. Run ./mysh to enter the my shell.
*/

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <limits.h>
#include <sys/wait.h>
#include <fcntl.h>

/* Definitions */
#define MAX_PROMPT_LENGTH 1024
#define TRUE 1
#define FALSE 2

/* Prototyping */
void spawn_first_child(int arg_count, char *parsed_args[MAX_INPUT][MAX_INPUT]);
void fork_for_pipes(int n, char *command[MAX_INPUT][MAX_INPUT]);
void start_child_process(char *parsed_args[MAX_INPUT][MAX_INPUT], int old_dup, int new_dup);
int parse_input(char input_stream[MAX_INPUT], char *parsed_args[MAX_INPUT][MAX_INPUT]);
void print_prompt();
void organize_redirection(char *parsed_args[MAX_INPUT][MAX_INPUT]);


int main(int argc, char *argv[]){
    char input_stream[MAX_INPUT];
    char *parsed_args[MAX_INPUT][MAX_INPUT];
    int arg_count;

    /* Get and print prompt */
    print_prompt();

    /* Stay in the shell until "exit" has been entered */
    while(1){

        /* Get input from standard in */
        if (fgets(input_stream, MAX_INPUT, stdin) == 0){
            printf("\n");
            break;
        }

        /* Print new prompt if the only input is a new line */
        if(strcmp(input_stream, "\n") == 0){
            print_prompt();
        }
        else{
            /* Parse the input string and pass it as an array of strings */
            arg_count = parse_input(input_stream, parsed_args);

            /* Exit if the string "exit" has been entered */
            if(strcmp(parsed_args[0][0], "exit") == 0){
                break;
            }

            /* organize the starting the processses */
            spawn_first_child(arg_count, parsed_args);
        }        
    }
}


void spawn_first_child(int arg_count, char *parsed_args[MAX_INPUT][MAX_INPUT]){
    /* Organizes the starting the child processes */

    pid_t child_pid;
    int exit_value;

    child_pid = fork();
    /* Start a child process */


    if(child_pid == 0){
        /* Child process */
        if(arg_count > 1){
            fork_for_pipes(arg_count, parsed_args);
        }
        else{
            /* NOW WE NEED TO ORGANIZE IF THERE ARE ANY REDIRECTION */
            organize_redirection(parsed_args);
        }
    }
    else {
        /* Parent process */

        wait(&exit_value);
        /* Print prompt again after child process is done */
        print_prompt();
    }
}

void organize_redirection (char *parsed_args[MAX_INPUT][MAX_INPUT]){
    int opened_file_descriptor;
    int plain_function_call = TRUE;
    int index = 0;

    while(parsed_args[0][index] != NULL){
        if(strcmp(parsed_args[0][index], "<") == 0){
            /* Input from file case */
            parsed_args[0][index] = NULL;
            if ((opened_file_descriptor = open(parsed_args[0][index+1], O_RDONLY)) == -1){
                perror("open");
            }
            start_child_process(parsed_args, opened_file_descriptor, 0);
            close(opened_file_descriptor);
            plain_function_call = FALSE;
            break;
        }
        else if(strcmp(parsed_args[0][index], ">") == 0){
            /* Output to file case */
            parsed_args[0][index] = NULL;
            if ((opened_file_descriptor = open(parsed_args[0][index+1], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1){
                perror("open");
            }
            start_child_process(parsed_args, opened_file_descriptor, 1);
            close(opened_file_descriptor);
            plain_function_call = FALSE;
            break;
        }
        else if(strcmp(parsed_args[0][index], ">>") == 0){
            /* Append to file case */
            parsed_args[0][index] = NULL;
            if ((opened_file_descriptor = open(parsed_args[0][index+1], O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1){
                perror("open");
            }
            start_child_process(parsed_args, opened_file_descriptor, 1);
            close(opened_file_descriptor);
            plain_function_call = FALSE;
            break;
        }
        index ++;
    }

    if(plain_function_call == TRUE){
        /* Dosnt add any dup links */
        start_child_process(parsed_args, 0, 0);
    }
    exit(errno);
}

void fork_for_pipes(int number_of_args, char *command[MAX_INPUT][MAX_INPUT]){
    int input = 0;
    int pipe_ends[2];
    int opened_file_descriptor;

    /* Determining where the first file should get its input from */

    /* check and parse input redirection */
    int index = 0;
    while(command[0][index] != NULL){
        if(strcmp(command[0][index], "<") == 0){

            command[0][index] = NULL;
            if ((opened_file_descriptor = open(command[0][index+1], O_RDONLY)) == -1){
                perror("open");
            }

            /* Set up the read from the file desriptor rather than the stdin */
            input = opened_file_descriptor;
            break;
        }
        index ++;
    }

    /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
    int current_arg;
    for(current_arg = 0; current_arg < number_of_args - 1; ++current_arg){
        pipe(pipe_ends); /* Get a pipe */

        if(fork()== 0){
            /* Parent process */

            /* Only change the fd if it is not the first in the pipe */
            if(input != 0){
                dup2(input, 0);
                close(input);
            }

            if(pipe_ends[1] != 1){
                dup2(pipe_ends[1], 1);
                close(pipe_ends[1]);
            }

            if (execvp(command[current_arg][0], (char * const *)command[current_arg]) == -1){
                perror ("execvp");
            }
        }
        close(pipe_ends[1]);

        /* pass on the read end of the pipe to the next child as the head */
        input = pipe_ends[0];
    }

    /* Set standard in to be the read end of the previous pipe */  
    if(input != 0) {
        dup2(input, 0);
    }

    index = 0;
    while(command[current_arg][index] != NULL){
        if(strcmp(command[current_arg][index], ">") == 0){
            /* Output to file case */
            command[current_arg][index] = NULL;
            if ((opened_file_descriptor = open(command[current_arg][index+1], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1){
                perror("open");
            }
            dup2(opened_file_descriptor, 1);
            close(opened_file_descriptor);
            break;
        }
        else if(strcmp(command[current_arg][index], ">>") == 0){
            /* Append to file case */
            command[current_arg][index] = NULL;
            if ((opened_file_descriptor = open(command[current_arg][index+1], O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1){
                perror("open");
            }
            dup2(opened_file_descriptor, 1);
            close(opened_file_descriptor);
            break;
        }
        index ++;
    }

    /* Finally another execvp */
    if (execvp(command[current_arg][0], (char * const *)command[current_arg]) == -1){
        perror("execvp");
    }
}

void start_child_process(char *parsed_args[MAX_INPUT][MAX_INPUT], int old_dup, int new_dup){
    /* "Routing" links accordingly */
    if(old_dup != new_dup){
        dup2(old_dup, new_dup);
    }

    /* Does the exicution */
    if (execvp(parsed_args[0][0], (char * const *)parsed_args[0]) == -1){
        perror("execvp");
    }
}


int parse_input(char input_stream[MAX_INPUT], char * parsed_args[MAX_INPUT][MAX_INPUT]){
    /* Parses input commands and separates into 2d array of strings */
    char ** args = NULL;
    int args_iter = 0;
    int vert_iter = 0;
    int horiz_iter = 0;

    /* shouldnt have to relinitialize data as null */
    for(int i = 0; i < MAX_INPUT; i ++){
        for(int j = 0; j < MAX_INPUT; j ++){
            parsed_args[i][j] = NULL;
        }
    }

    input_stream[strcspn(input_stream, "\n")] = 0;

    /* Splits the input string based on spaces */
    char * split_input = strtok(input_stream, " ");

    while(split_input){
        args = realloc(args, sizeof(char*) * ++args_iter);
        args[args_iter-1] = split_input;
        split_input = strtok(NULL, " ");
    }

    /* realloc one extra element for the last NULL */
    args = realloc(args, sizeof(char*) *(args_iter+1));
    args[args_iter] = NULL;

    /* Layering the args split tokens into the 2d array */
    for(int i = 0; i <(args_iter); ++i){
        if(strcmp(args[i], "|") == 0){
            vert_iter ++;
            horiz_iter = 0;
            parsed_args[vert_iter][horiz_iter] = NULL;
        }
        else{
            parsed_args[vert_iter][horiz_iter] = args[i];
            horiz_iter ++;
        }
    }

    /* free the memory allocated */
    free(args);

    return vert_iter + 1;
}


void print_prompt(){
    /* Gets the prompt from system */
    struct passwd *pwd;
    char name[32];
    char hostname[HOST_NAME_MAX + 1];
    char path[PATH_MAX];

    pwd = getpwuid(getuid());
	if(pwd == NULL){
		sprintf(name, "%d", getuid());
	}
	else {
		sprintf(name, "%s", pwd->pw_name);
	}

    gethostname(hostname, HOST_NAME_MAX + 1);
    getcwd(path, PATH_MAX);

    /* Creating the string and setting the color of the prompt */
    printf("\033[0;32m%s\033[0m@\033[0;32m%s\033[0m:\033[0;35m%s"
        "\033[0m$ ", name, hostname, path);
}
