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
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include "line_parser.h"

#define PORT "2018"
#define TRUE 1
#define FALSE 0
#define BUFF_SIZE 2048

int bye_helper();
int ls_helper();
int conn_helper();
int client_loop();
int nok_helper(char*);

client_state * cs;
int id; 
int sock_fd;
int debug_flag;
int client_fd;
int main(int argc, char * argv[]){
    int i;
    debug_flag = 0;
    for (i=0; i<argc; i++){
        if (strcmp (argv[i],"-d")==0){
            debug_flag = 1;
        }
    }
    id = 1;
    cs = create_client();
    char host_name[BUFF_SIZE];
    gethostname(host_name,BUFF_SIZE);
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;  // will point to the results
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) < 0) {
        return -1;
    }
    else{
        sock_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);         
        if (sock_fd==-1){
            return -1;
        }
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof their_addr;
        cs->sock_fd = sock_fd;
        while(TRUE){
            char * addr = calloc (BUFF_SIZE, sizeof(char));
            strcpy(addr,host_name);
            bind(sock_fd, servinfo->ai_addr, servinfo->ai_addrlen);
            listen (sock_fd,1);
            cs->server_addr = addr;
            int new_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &addr_size);
            client_fd = new_fd;
            client_loop();
            
        }
    }
    free(cs);
    return 0;
}

int client_loop(){
    int recv_bytes;
    int succ = -1;
    int was_bye = 0;
    while (TRUE){
        char buffer[BUFF_SIZE] = {0};
        if ((recv_bytes = recv(client_fd,(void*)buffer,BUFF_SIZE,0))>0){
            debug_print(debug_flag, buffer, cs->server_addr);
            cmd_line * cmd = parse_cmd_lines(buffer);
            switch(cmd->arg_count){
                case 1:
                    if (strncmp (cmd->arguments[0],"hello",5)==0){
                        succ = conn_helper();
                    }
                    else if (strncmp(cmd->arguments[0],"ls",2)==0){
                        succ = ls_helper();
                    }
                    else if (strncmp(cmd->arguments[0],"bye",3)==0){
                        succ = bye_helper();
                        was_bye = 1;
                    }
                    else{
                        succ = nok_helper(buffer);
                    }
                    break;
                default:
                    succ = nok_helper(buffer);
                    break;
            }
            if (succ!=0 || was_bye){
                was_bye = 0;
                break;
            }
        }
        else{
            break;
        }
    }
    return succ;
}

int conn_helper(){
    if (cs->conn_state==IDLE){
        char respond[BUFF_SIZE] = {0};
        sprintf(respond, "hello %d\n", id);
        printf("Client %d connected\n",id);
        int bytes_send = send(client_fd, (void*)respond, strlen(respond),0); 
        if (bytes_send<1){
            return -1;
        }
        cs->sock_fd = client_fd;
        cs->conn_state = CONNECTED;
        cs->client_id = calloc (BUFF_SIZE, sizeof(char));
        sprintf(cs->client_id, "%d", id);
        return 0;
    }
    else{
        return -1;
    }
}

int ls_helper(){
    if (cs->conn_state == CONNECTED){
        char * first_respond = "ok\n";
        int bytes_send = send(client_fd, (void*)first_respond, strlen(first_respond),0); 
        if (bytes_send<1){
            return -1;
        } 
        char * my_list_dir = list_dir();
        bytes_send = send(client_fd, (void*)my_list_dir, strlen(my_list_dir),0); 
        if (bytes_send<1){
            return -1;
        }
        return 0;
    }
    else{
        return -1;
    }
}

int bye_helper(){
    if (cs->conn_state == CONNECTED){
        close(client_fd);
        printf("Client %d disconnected\n", id);
        set_initial_state(cs);
        id++; 
        return 0;
    }
    else{
        return -1;
    }    
}

int nok_helper (char * buffer){
    char * respond = "nok\n";
    fprintf(stderr, "%d|ERROR: Unknown message %s\n", id, buffer);
    int bytes_send = send(client_fd, (void*)respond, strlen(respond),0); 
    if (bytes_send<1){
        return -1;
    }
    close(client_fd);
    printf("Client %d disconnected\n", id);
    set_initial_state(cs);
    id++;
    return -1;
}
