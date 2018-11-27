#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include "common.h"


static char* _list_dir(DIR * dir, int len) {
	struct dirent * ent = readdir(dir);
	struct stat info;

	if (ent == NULL) {
		char * listing = malloc((len + 1) * sizeof(char));
		listing[len] = '\0';
		return listing;
	}
	if (stat(ent->d_name, &info) < 0) {
		perror("stat");
		return NULL;
	}
	
	int ent_len = 0;
	if(S_ISREG(info.st_mode)){
		ent_len = strlen(ent->d_name);
		ent_len++; /* Account for the \n */
	}
	
	char * listing = _list_dir(dir, len + ent_len);	
	
	if(S_ISREG(info.st_mode)){	
		strcpy(&listing[len], ent->d_name);
		listing[len + ent_len - 1] = '\n';
	}
	return listing;
}


long file_size(char * filename){
    long filesize = -1;
    FILE * file;
    file = fopen (filename, "r");
    if (file == NULL){
            perror("fopen");
            goto err;
    }
    
    if(fseek (file, 0, SEEK_END) != 0){
        perror("fseek");
        goto err;
    }
    if((filesize = ftell(file)) < 0){
            perror("ftell");
            goto err;
    }		
    
    fclose(file);
    
    return filesize;
    
    err:
    if (file) {
            fclose(file);
    }
    return -1;
}

//@return must be freed by the caller
char* list_dir(){
	DIR *dir = opendir("."); 
	if (dir == NULL){
		perror("opendir");
		return NULL;
	}
	
	char * listing = _list_dir(dir, 0);
	closedir(dir);
	return listing;
}
client_state * create_client(){
    client_state * cs = calloc (1, sizeof(client_state));
    cs->server_addr = "nil";
    cs->conn_state = IDLE;
    cs->client_id = NULL;
    cs->sock_fd = -1;
    return cs;
}
void set_initial_state (client_state * cs){
    free_client_state(cs);
    cs->server_addr = "nil";
    cs->conn_state = IDLE;
    cs->client_id = NULL;
    cs->sock_fd = -1;
}
void free_client_state (client_state * cs){
    if (cs->server_addr!=NULL&&strcmp(cs->server_addr,"nil")!=0){
        free(cs->server_addr);
    }
    if (cs->client_id!=NULL){
        free (cs->client_id);
    }
}
void debug_print (int debug_flag, char * msg, char * info){
    if (debug_flag){
        fprintf(stderr, "%s|Log: %s", info, msg);
    }
}
