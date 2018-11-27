#include <sys/types.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DONE -1
#define RUNNING 1
#define SUSPENDED 0
#define CONTINUE 1
#define TRUE 1
#define FALSE 0

typedef struct job
{
    char *cmd;
    int idx;				/* index of current job (1 for the first) */
	pid_t pgid; 		/* save group id of the job*/
	int status; /* 1 running 0 suspendid -1 Done  TODO: see if there's a need for it print done jobs before a command is performed*/
	struct termios *tmodes; /* saved terminal modes */
    struct job *next;	/* next job in chain */
} job;

/* implemented functions */
job* add_job(job** job_list, char* cmd);
void remove_job(job** job_list, job* tmp);
void print_jobs(job** job_list);
void free_job_list(job** job_list);
char* status_to_str(int status);


/* to be implemented during the lab */
job* initialize_job(char* cmd);
void free_job(job* job_to_remove);
job* find_job_by_index(job * job_list, int idx);
void update_job_list(job **job_list, int remove_done_jobs);
void run_job_in_foreground (job** job_list, job *j, int cont, struct termios* shell_tmodes, pid_t shell_pgid);
void run_job_in_background (job *j, int cont);


