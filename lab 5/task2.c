#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "line_parser.h"
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>
#include <sys/types.h>

#ifndef NULL
    #define NULL 0
#endif

#define FREE(X) if(X) free((void*)X)

int main (){
    int status, status2;
    int fd[2];
    pipe (fd);
    int pid = fork (), pid2;
    if (pid == 0){
        close(stdout);
        dup2(fd[1],1);// dup2 automatically closing fd[1]
        char *first[3] = {"ls", "-l", 0};
        execvp ("ls",first);
    }
    else{
        close(fd[1]);
        if ((pid2 = fork())==0){
        close(stdin);
        dup2(fd[0], 0); // dup2 automatically closing fd[0]
        char *sec[4] = {"tail", "-n", "2", 0};
        execvp ("tail",sec);
        }
        else{
            close (fd[0]);
            waitpid (pid, &status, 0);
            waitpid (pid2, &status2, 0);
        }
    }
    return 0;
}