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


int execute (cmd_line *line){
    if (line->input_redirect!=NULL){
        close(0);
        if((fopen(line->input_redirect,"r"))==NULL){
            return -1;
        }
    }
    if (line->output_redirect != NULL){
        close(1);
        if ((fopen(line->output_redirect, "w+"))==NULL){
            return -1;
        }
    }
    return(execvp((line->arguments)[0], line->arguments));
}
void fork_sec(cmd_line * old, cmd_line * cmd_line,int old_pid, int * fd){
    int status, status2;
    close(fd[1]);
    int pid = fork();
    if (pid==0){
        close(stdin);
        dup2(fd[0],0);
        execute(cmd_line);
    }
    else{
        close(fd[0]);
        waitpid(old_pid,&status,0);
        waitpid(pid,&status2,0);
    }
}
int fork_one (cmd_line * cmd_line){
    int status;
    int fd[2];
    pipe(fd);
    if (cmd_line==NULL){
        return -1;
    }
    int pid = fork();
    switch(pid){
         case -1:
            free_cmd_lines(cmd_line);
            return -1;
        case 0:
            if (!(cmd_line->next==NULL)){
                close(stdout);
                dup2(fd[1],1);
            }
            if (execute(cmd_line)==-1){
                free_cmd_lines(cmd_line);
                perror("error!\n");
                return -1;// using _exit and not exit, because we want to quit the child process and not the parent
            }
            break;
        default:
            if (!(cmd_line->next==NULL)){
                fork_sec(cmd_line,cmd_line->next, pid ,fd);
            }
            else{
                waitpid (pid, &status, cmd_line->blocking);
            }
            break;
    }
    return 0;
}
int main (){
    while (1){
        char buf[PATH_MAX];
        getcwd(buf, PATH_MAX);
        printf("%s/", buf);
        char str[2048]; // max buff size
        char * from_user = fgets(str, 2048, stdin);
        if (strcmp(from_user, "quit\n")==0){
            _exit(0);
        }
        if (strcmp(from_user, "\n")==0){
            continue;
        }
        cmd_line * cmd_line = parse_cmd_lines (from_user);
        if(fork_one(cmd_line)==-1){
            _exit(errno);
        }
            free_cmd_lines(cmd_line);
        }
        return 0;
}