#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <dirent.h>
#include <ctype.h>
#include "common.h"
#include "line_parser.h"
#include <arpa/inet.h>

void print_server_data ();
int bye_helper();
int conn_helper(char *);
int exec(cmd_line *);
int send_cmd (cmd_line *);
int general_func(char*);
int ls_helper();
int get_helper(char*);
long find_slash_n(char*);

client_state * cs;
int debug_flag;

struct timeval tv;

#define PORT "2018"
#define TRUE 1
#define FALSE 0
#define BUFF_SIZE 2048
#define GET_FILE_BUFFER 12
#define OK_BUFFER 4

int main (int argc, char* argv[]){
    int i;
    debug_flag = 0;
    for (i=0; i<argc; i++){
        if (strcmp(argv[i],"-d")==0){
            debug_flag = 1;
        }
    }
    cs = create_client();
    while(TRUE){
        print_server_data();
        char command[BUFF_SIZE];
        fgets(command, BUFF_SIZE, stdin);
        if (strcmp(command,"quit\n")==0){
            free_client_state(cs);
            free(cs);
            break;
        }
        else{
            cmd_line * cmd = parse_cmd_lines(command);
            if (cmd!=NULL){
                exec(cmd);
                free_cmd_lines(cmd);
            }
            else{
                printf("Parse command error!\n");
            }
        }
    }
    return 0;
}

int conn_helper(char * ip){
    if (cs->conn_state!=IDLE){
        return -2;
    }
    else{ 
        int sock_fd;
        int status;
        struct addrinfo hints;
        struct addrinfo *servinfo;
        memset (&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if ((status = getaddrinfo(ip, PORT, &hints, &servinfo))!=0){
            fprintf(stderr, "getaddrinfo error: %s\n",gai_strerror(status));
            exit (-1);
        }

        if ((sock_fd = socket (servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol))==-1){
            return -1; // socket error
        }
        if((connect(sock_fd, servinfo->ai_addr , servinfo->ai_addrlen))<0){
            return -1; // connect error
        }
        freeaddrinfo(servinfo);
        tv.tv_sec = 1;
        setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        char * to_server = "hello\n";
        if ((send(sock_fd, to_server,strlen(to_server), 0))<0){
            return -1; // failed to send
        }
        char buffer[BUFF_SIZE];
        if ((recv(sock_fd, buffer, BUFF_SIZE,0))<1){
            printf("recv fail\n");
            cs->conn_state = IDLE;
            return -1;
        }
        cmd_line * server_ans = parse_cmd_lines(buffer);
        if (strcmp(server_ans->arguments[0],"hello")==0){
            debug_print(debug_flag, buffer, cs->server_addr);
            if (server_ans->arg_count>1){
                cs->sock_fd = sock_fd;
                cs->conn_state = CONNECTED;
                char * client_id = calloc (strlen(server_ans->arguments[1]),sizeof(char));
                strcpy(client_id,server_ans->arguments[1]);
                cs->client_id = client_id;
                char * ip_addr = calloc (strlen(ip)+1, sizeof(char));
                strcpy(ip_addr,ip);
                cs->server_addr = ip_addr;
                free_cmd_lines(server_ans);
                return 0;
            }
            else{
                free_cmd_lines(server_ans);
                return -1; // server didn't send client_id
            }
        }
        else{
            cs->conn_state = IDLE;
            return -1;
        }
    }
}

int ls_helper(){
    int sock_fd = cs->sock_fd;
    char * to_server = "ls\n";
    if ((send(sock_fd, to_server,strlen(to_server), 0))<0){
        return -1; // failed to send
    }
    char stat[OK_BUFFER] = {0};
    char * buffer = calloc (BUFF_SIZE, sizeof(char));
    if ((recv(sock_fd, stat, OK_BUFFER,0))<1){
        return -1;
    }
    debug_print(debug_flag, stat, cs->server_addr);
    if (strncmp("ok", stat, 2)==0){
        if ((recv(sock_fd, buffer, BUFF_SIZE,0))<1){
            free (buffer);
            return -1;
        }
        else{
            printf("%s",buffer);
            free(buffer);
            return 0;
        }
    }
    else{
        close(cs->sock_fd);
        set_initial_state(cs);
        return -1;
    }    
}
int general_func (char * to_server){
    int sock_fd = cs->sock_fd;
    if ((send(sock_fd, to_server,strlen(to_server), 0))<0){
        return -1; // failed to send
    }
    char stat[BUFF_SIZE] = {0};
    if ((recv(sock_fd, stat, BUFF_SIZE,0))<1){
        return -1;
    }
    debug_print(debug_flag, stat, cs->server_addr);
    if (strncmp("nok", stat, 3)==0){
        close(cs->sock_fd);
        set_initial_state(cs);
        return 0;
    }
    else{
        return -1;
    }
}
int get_helper(char * file_name){
    int sock_fd = cs->sock_fd;    
    tv.tv_sec = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    long file_size, bytes_recived = 0;
    FILE * file;
    char to_server[BUFF_SIZE] = "get ";
    strcat(to_server,file_name);
    strcat(to_server, "\n");

    if ((send(sock_fd, to_server,strlen(to_server), 0))<0){
        return -1; // failed to send
    }
    char tmp_stat[BUFF_SIZE] = {0};
    char stat[BUFF_SIZE] = {0};
    char buffer[BUFF_SIZE] = {0};
    if ((recv(sock_fd, tmp_stat, BUFF_SIZE,MSG_PEEK))<1){
        return -1;
    }
    long OK_length = find_slash_n(tmp_stat);
    if ((recv(sock_fd, stat, OK_length+1,0))<1){
        return -1;
    }
    cmd_line * server_ans = parse_cmd_lines(stat);
    debug_print(debug_flag, stat, cs->server_addr);
    if (strcmp("ok", server_ans->arguments[0])==0){
            if (server_ans->arg_count>1){
                file_size = atol (server_ans->arguments[1]);
                free_cmd_lines(server_ans);
                if (file_size>0){
                    char tmp_file_name[BUFF_SIZE] = {0};
                    strcpy(tmp_file_name,file_name);
                    strcat(tmp_file_name, ".tmp");
                    if ((file=fopen(tmp_file_name,"w"))!=NULL){
                        cs->conn_state = DOWNLOADING;
                        memset(buffer,0,BUFF_SIZE);                        
                        while (((bytes_recived=recv(sock_fd,(void*)buffer,BUFF_SIZE,0))>0)&&file_size>0){ 
                            fwrite(buffer, sizeof(char),bytes_recived,file);
                            file_size -= bytes_recived;
                            memset(buffer,0,BUFF_SIZE);
                        }
                        char * done = "done\n";
                        if ((send(sock_fd, done, strlen(done), 0))<0){
                            return -1; // failed to send
                        }
                        memset(buffer,0,BUFF_SIZE);
                        tv.tv_sec = 10;
                        setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
                        if ((recv(sock_fd, (void*)buffer, 4, 0))<0){
                            // recv fail
                            remove(tmp_file_name);
                            printf("Error while downloading file %s", file_name);
                            close(cs->sock_fd);
                            set_initial_state(cs);
                            return -1;
                        }
                        server_ans = parse_cmd_lines(buffer);
                        if (server_ans->arg_count>0){
                            if (strncmp(server_ans->arguments[0],"ok",2)==0){
                                rename(tmp_file_name,file_name);
                                cs->conn_state = CONNECTED;
                                fclose(file);
                                return 0;
                            }
                            else{ //nok
                                remove(tmp_file_name);
                                printf("Error while downloading file %s", file_name);
                                close(cs->sock_fd);
                                set_initial_state(cs);    
                                return -1;                            
                            }
                        }
                        else{ // empty response
                            remove(tmp_file_name);
                            printf("Error while downloading file %s", file_name);
                            close(cs->sock_fd);
                            set_initial_state(cs);                            
                        }
                    }
                    else{
                        printf("File creation error\n");
                        return -1;
                    }
                }
                else{
                    printf ("Illegal file size\n");
                    return -1;
                }
            }
            else{
                printf ("Undefined size :(\n");
                return -1;
            }
    }
    else{ //nok
        close(cs->sock_fd);
        set_initial_state(cs);
        return -1;
    }
    return 0;
}
int send_cmd (cmd_line * cmd){
    if (cs->conn_state==CONNECTED){
        if (strcmp("ls",cmd->arguments[0])==0){
            return ls_helper();
        }
        else if (strcmp("get",cmd->arguments[0])==0){
            if ((cmd->arg_count)>1){
                return get_helper(cmd->arguments[1]);
            }
            else{
                printf("Please provide file name\n");
                return -1;
            }
        }
        else{
            return general_func(cmd->arguments[0]);
        }
    }
    else{
        return -2;
    }
}

void print_server_data (){
    if (cs!=NULL){
        printf("server:%s> ",cs->server_addr);
    }
    else{
        perror("client_state not initiallized!\n");
    }
}
int bye_helper(){
    if (cs->conn_state==CONNECTED){
        char * bye = "bye\n";
        int bytes_send = send(cs->sock_fd,bye, strlen(bye), 0); 
        if (bytes_send<1){ // connected, but failed to send "bye" to the server
            return -1;
        }
        else{
            close(cs->sock_fd);
            set_initial_state(cs);
            return 0;
        }
    }
    else{
        return -2; // not connected, so we can't disconnect...
    }
}

int exec(cmd_line * cmd){
    if (strcmp (cmd->arguments[0],"bye")==0){
        return bye_helper();
    }
    else if (strcmp(cmd->arguments[0],"conn")==0 && cmd->arg_count==2){
        return conn_helper(cmd->arguments[1]);
    }
    else{
        return send_cmd(cmd);
    }
}

long find_slash_n(char * str){
    long i = 0;
    while (str[i] != '\n'){
        i++;
    }
    return i;
}