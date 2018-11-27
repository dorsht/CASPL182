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
        fclose(stdin);
        if((fopen(line->input_redirect,"r"))==NULL){
            return -1;
        }
    }
    if (line->output_redirect != NULL){
        fclose(stdout);
        if ((fopen(line->output_redirect, "w"))==NULL){
            return -1;
        }
    }
    return(execvp((line->arguments)[0], line->arguments));
}
int main (){
    while (1){
        int pid;
        int status;
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
        if (cmd_line == NULL) {
            _exit(1);
        }
        else{
            if((pid=fork ())==-1){ // fork child process
                free_cmd_lines(cmd_line);
                _exit(1); // in case of failure
            }
            else{ 
                if (pid==0){ // child process
                    if(execute(cmd_line)==-1){
                        free_cmd_lines(cmd_line);
                        perror("error!\n");
                        _exit(errno); // using _exit and not exit, because we want to quit the child process and not the parent
                    }
                }
                else{ // Wait
                    waitpid(pid,&status,cmd_line->blocking);
                }
                free_cmd_lines(cmd_line);
            }
        }  
    }
    return 0;
}