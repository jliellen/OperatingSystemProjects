/**
 * Simple shell interface starter kit program.
 * Operating System Concepts
 * Mini Project1
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

int main(void)
{
    char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    int should_run = 1;
    char text[MAX_LINE] = {""}, commandLine[MAX_LINE] = {""};
    pid_t childpid, child_childpid;
    int fd, ret;
    int pipefd[2];


    while (should_run){
        printf("mysh:~$ ");
        fflush(stdout);
        /**
          * After reading user input, the steps are:
          * (1) fork a child process
          * (2) the child process will invoke execvp()
          * (3) if command includes &, parent and child will run concurrently
          */
        fgets(text, MAX_LINE, stdin); //read user's input ending with '\n'

        if (strcmp(text, "exit\n") != 0) 
        {
            //change the last character '\n' to '\0'
            text[strlen(text)-1] = '\0';

            //run last command line by typing "!!"
            if (strcmp(text, "!!") == 0) 
            {
                if (strlen(commandLine) == 0) {
                    printf("%s\n", "No commands in history.");
                    continue;
                } else {
                    memcpy(&text, commandLine, strlen(commandLine)+1); //copy the string
                }
            }
            memcpy(&commandLine, text, strlen(text)+1); //save history command

            int shouldWait = 0;
            if (text[strlen(text)-1] == '&') {
                text[strlen(text)-1] = '\0';
                shouldWait = 1;
            }

            childpid = fork();
            if(childpid == -1)
            {
                perror("fork");
                exit(1);
            }
            if (childpid != 0)
            {
                //parent process
                wait(NULL);
            } 
            else 
            {
                //first child process
                //tokenize user's input
                args[0] = strtok(text, " ");
                int i = 0;
                while (args[i] != NULL) {
                    args[++i] = strtok(NULL, " ");
                }

                if (shouldWait) { 
                    setpgid(0, 0);
                    exit(1);
                } 
        
                i--; //index points to the last word, avoid nullpointer exception
                
                while (i >= 0) {
                    // check if the case is using pipe
                    if (**(args+i) == '|') {
                        args[i] = NULL;
                        pipe(pipefd);   
                        if(pipe(pipefd) == -1)
                        {
                            perror("pipe");
                            exit(1);
                        }
                        
                        child_childpid = fork();

                        if (child_childpid == 0) { 
                            dup2(pipefd[0], 0);
                            close(pipefd[1]);
                            execvp(args[i+1], &args[i+1]);                     
                        } else {
                            dup2(pipefd[1], 1);
                            close(pipefd[0]);
                            execvp(args[0], args);
                        }
                        perror("execvp");
                        exit(1);
                    }

                    // check if the case is redirecting output
                    else if (**(args+i) == '>') {
                        args[i] = NULL;
                        fd = open(args[i+1], O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWUSR);
                        if (fd < 0) {
                            perror("open");
                            exit(1);
                        }
                        ret = dup2(fd, 1);
                        if (ret < 0) {
                            perror("dup2");
                            exit(1);
                        }
                    }

                    // check if the case is redirecting input
                    else if (**(args+i) == '<'){
                        args[i] = NULL;
                        fd = open(args[i+1], 0644);
                        if (fd < 0) {
                            perror("open");
                            exit(1);
                        }
                        ret = dup2(fd, 0);
                        if (ret < 0) {
                            perror("dup2");
                            exit(1);
                        }
                    }

                    i--; // move index
                } 
             
                //excute command line
                execvp(args[0], args);
                //kill child. Failed to execute command
                perror("execvp");
                return 1;
            }
        
        } else { //user input "exit"
            should_run = 0;
        }
    }

    return 0;
}