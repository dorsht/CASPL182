#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>
#include <sys/types.h>
#include "job_control.h"
#include "line_parser.h"
#include <termios.h>


#ifndef NULL
    #define NULL 0
#endif

#define FREE(X) if(X) free((void*)X)
/* this method helps to check foreground,
 * and returns the number of the job that we need to run foreground,
 * if given in the string*/
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
int new_execute (cmd_line * cmd_line, int * right_fd, int * left_fd, job * temp, int change_temp_pid){
    if (cmd_line==NULL){
        return -1;
    }
    int status;
    int * new_fd = calloc(2, sizeof(int));
    int pid = fork();
    int curr = pid;
    switch (pid){
        case -1:
            free_cmd_lines(cmd_line);
            free(new_fd);
            return -1;
            break;
        case 0:
            if (temp->pgid == 0){
                temp->pgid = getpid();
            }
                
            setpgid (getpid(),temp->pgid);
            if (change_temp_pid==0){
                temp->pgid = curr;
                change_temp_pid = 1;
            }
            if (right_fd != NULL && cmd_line->next != NULL){
                close(1);
                dup2(right_fd[1],1);
            }
            if (left_fd != NULL){
                close(0);
                dup2(left_fd[0], 0);
            }
            execute(cmd_line);
            break;
        default: //Parent
            if (temp->pgid == 0){
                temp->pgid = getpid();
            }
            setpgid (curr,temp->pgid);
            if (right_fd != NULL){
                close(right_fd[1]);
            }
            if (left_fd != NULL){
                close (left_fd[0]);
            }
            if (cmd_line->next != NULL){
                pipe (new_fd);
                new_execute(cmd_line->next, new_fd, right_fd,temp,change_temp_pid);
            }
            else{
                while(waitpid(pid, &status, WNOHANG)!=-1);
                temp->status = -1;
            }
            break;
    }
    free(new_fd);
    return 0;
}
int main (){
    int pid;
    setpgid (getpid(), getpid());
    job ** list = calloc (100, sizeof(job));
    job *temp, *joby;    
    struct termios old;
    signal(SIGQUIT, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    while (1){
        char path[2048];
        getcwd(path, 2048);
        printf("%s/", path);
        tcgetattr(STDIN_FILENO, &old);
        char str[2048]; // max buff size
        char * from_user = fgets(str, 2048, stdin);
        if (strcmp(from_user, "quit\n")==0){
            free_job_list(list);
            free(list);
            _exit(0);
        }
        else if (strcmp(from_user, "\n")==0){
            continue;
        }
        else if (strcmp(from_user, "jobs\n")==0){
            printf(" --- Jobs list --- \n");
            print_jobs(list);
            continue;
        }
        else if (strncmp(from_user, "fg ", 3)==0){
            int idx = atoi(&from_user[3]);
            if (!idx){
                printf("bad input!\n");
            }
            else{
                if ((joby = find_job_by_index(*list,idx))!=NULL){
                    run_job_in_foreground(list, joby, 1, &old, getpgid(getpid()));
                }
                continue;
            }
        }
        else{
                temp = add_job(list, from_user);
                temp->status = 1;
                tcgetattr(STDIN_FILENO, temp->tmodes);
                pid = fork();
                cmd_line * cmd_line = parse_cmd_lines(from_user);
                switch (pid){
                    case 0:
                        setpgid(getpid(),getpid());
                        temp->pgid = getpid();
                        signal(SIGQUIT, SIG_DFL);
                        signal(SIGCHLD, SIG_DFL);
                        signal(SIGTSTP, SIG_DFL);
                        signal(SIGTTIN, SIG_DFL);
                        signal(SIGTTOU, SIG_DFL);
                        tcgetattr(STDIN_FILENO, &old);
                        int *fd = calloc (2, sizeof(int));
                        pipe(fd);
                        if (cmd_line->next == NULL){
                            if(new_execute(cmd_line,NULL, NULL,temp,0)==-1){
                                _exit(errno);
                            }   
                        }
                        else{
                            if(new_execute(cmd_line, fd,NULL,temp,0)==-1){
                                _exit(errno);
                            }
                        }
                        free_cmd_lines(cmd_line);
                        free (fd);
                        _exit(0);
                        break;
                    case -1:
                        free_cmd_lines (cmd_line);
                        _exit(errno);
                        break;
                    default:
                        temp->pgid = pid;
                        setpgid(pid,pid);
                        if (cmd_line->blocking){
                            run_job_in_foreground(list, temp, 0, &old, getpgid(getpid()));
                        }
                        else{
                            run_job_in_background(temp, 0);
                        }
                        break;
                }
                free_cmd_lines(cmd_line);
            
        }
    }
    return 0;
}
