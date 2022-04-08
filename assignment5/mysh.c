/*
* mysh.c
*
* Second attempt at implementation of a shell. Run ./mysh2 to enter the my shell.
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
#define READ_END 0
#define WRITE_END 1
#define TRUE 1
#define FALSE 2
#define MAX_INPUT_LENGTH 4096

/* Prototyping */
void print_prompt();
int handle_pipes(char input_stream[MAX_INPUT_LENGTH]);

int main(int argc, char *argv[]){
    char input_stream[MAX_INPUT_LENGTH];
    int exit_value;
    int num_children;

    /* Stay in the shell until "exit" has been entered */
    while (1){
        print_prompt();

        
        /* Get input from standard in */
        if (fgets(input_stream, MAX_INPUT_LENGTH, stdin) == 0){
            printf("\n");
            break;
        }

        /* Print new prompt if the only input is a new line */
        if (strcmp(input_stream, "\n") == 0){
        }

        input_stream[strcspn(input_stream, "\n")] = 0;
        if (strcmp(input_stream, "exit") == 0){
            break;
        }
        
        else{
            num_children = handle_pipes(input_stream);

            /* TODO wait for the correct number of chile processes */
            for (int i = 0; i < num_children; i ++){
                wait(&exit_value);
            }
        }      
    }
}

int handle_pipes(char input_stream[MAX_INPUT_LENGTH]){
    char *command[MAX_INPUT_LENGTH];
    char *split_on_pipe, *rest_on_pipe;
    char *split_on_space, *rest_on_space;
    int num_children = 0;

    /* Initializing values to determine the last command in a pipe sequence */
    char test_end_input_stream[MAX_INPUT_LENGTH];
    char *split_test_end, *rest_test_end;

    /* Initializing requirements for piping */
    int command_number = 0;
    int prev_pipe_ends[2];
    int next_pipe_ends[2];
    int start = TRUE;
    int opened_file_descriptor;
    pid_t pid;

    /* runs through the input stream once to coppy it so we can later determine the
    * last element of the pipe sequence */
    int stream_iter = 0;
    while (input_stream[stream_iter] != '0') {
        test_end_input_stream[stream_iter] = input_stream[stream_iter];
        stream_iter ++;
    }
    /* runs it once so it will be one slice ahead */
    split_test_end = strtok_r(test_end_input_stream, "|", &rest_test_end);

    /* splitting the input steam */
    for(split_on_pipe = strtok_r(input_stream, "|", &rest_on_pipe) ; split_on_pipe != NULL ; split_on_pipe = strtok_r(NULL, "|", &rest_on_pipe) ) {
        command_number ++;

        /* test to see if it is the last element of the pipe sequence */
        split_test_end = strtok_r(NULL, "|", &rest_test_end);

        /* splitting on spaces */
        int command_index = 0;
        for(split_on_space = strtok_r(split_on_pipe, " ", &rest_on_space) ; split_on_space != NULL ; split_on_space=strtok_r(NULL, " ", &rest_on_space)) {
            command[command_index] = split_on_space;
            command_index ++;
        }
        /* Have to add a finishing null */
        command[command_index] = NULL;

        /* Done parsing command will create pipe and fork*/

        if (start == TRUE) {
            /* only on the first time around do we not need to shuffly down the pipe ends */
            pipe(next_pipe_ends);
            start = FALSE;
        }
        else {
            /* we need to shuffle down the pipe ends so that they are ready for the next element of the pipe sequence */
            prev_pipe_ends[READ_END] = next_pipe_ends[READ_END];
            prev_pipe_ends[WRITE_END] = next_pipe_ends[WRITE_END];

            if (split_test_end != NULL){
                /* will not add new pipe on the last pipe in chain */
                pipe(next_pipe_ends);
            }
        }

        num_children ++;
        if ((pid = fork()) == 0){  
            /* Child Process */

            /* If it is the first command read from the stdin or file redirect, othewise read from pipe end */
            if (command_number == 1) {
                /* If it is the first command of the sequence check for input redirection */
                int index = 0;
                while (command[index] != NULL){
                    if (strcmp(command[index], "<") == 0){

                        /* Input from file case */
                        command[index] = NULL;
                        if ((opened_file_descriptor = open(command[index + 1], O_RDONLY)) == -1){
                            perror("open");
                        }
                        dup2(opened_file_descriptor, 0);
                        close(opened_file_descriptor);
                        break;
                    }
                    index ++;
                }
            }
            else {
                /* otherwise, no need to check for previous pipe ends just dup input */
                dup2(prev_pipe_ends[READ_END], 0);
                close(prev_pipe_ends[READ_END]);
            }

            /* If it is the last command, write to the stdout or file redirect, othewise write to the pipe end */
            if (split_test_end == NULL){
                /* if it is the last element of the pipe sequnece we check for output redirection */
                int index = 0;
                while (command[index] != NULL){
                    if (strcmp(command[index], ">") == 0){
                        /* Output to file case */
                        command[index] = NULL;
                        if ((opened_file_descriptor = open(command[index + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1){
                            perror("open");
                        }
                        dup2(opened_file_descriptor, 1);
                        close(opened_file_descriptor);
                        break;
                    }
                    else if (strcmp(command[index], ">>") == 0){
                        /* Append to file case */
                        command[index] = NULL;
                        if ((opened_file_descriptor = open(command[index + 1], O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1){
                            perror("open");
                        }
                        dup2(opened_file_descriptor, 1);
                        close(opened_file_descriptor);
                        break;
                    }
                    index ++;

                }
                close(next_pipe_ends[WRITE_END]);
            }
            else {
                /* otherwise we just do a normal dup to the input of the next pipe */
                dup2(next_pipe_ends[WRITE_END], 1);
                close(next_pipe_ends[WRITE_END]);
            }

            /* finally we do the actual exec after all the dups are setup */
            if (execvp(command[0], command) == -1){
                perror("execvp");
            }
            
        }
        else {
            close(next_pipe_ends[WRITE_END]);
        }
    }
    return num_children;
}


void print_prompt(){
    /* Gets the prompt from system */
    struct passwd *pwd;
    char name[32];
    char hostname[HOST_NAME_MAX + 1];
    char path[PATH_MAX];

    pwd = getpwuid(getuid());
	if (pwd == NULL){
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
