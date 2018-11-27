#include "lab4_util.h"

#define SYS_READ 0
#define SYS_WRITE 1
#define STDOUT 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_EXIT 60
#define SYS_CHROOT 61
#define SYS_GETDENTS 78

extern int system_call();
struct linux_dirent {
	long	d_ino;
	long	d_off;
	short	d_reclen;
	char   d_name[];
};
void create_new_path (char * old_path, char * file_name, char * new_path){
    int old = simple_strlen(old_path), add = simple_strlen(file_name);
    int size = 2 + old + add;
    int i, j;

    for (i=0; i<old; i++){
        new_path[i] = old_path[i];
    }
    new_path[i++] = '/';
    for (j = 0; j<add; j++){
        new_path[i++] = file_name[j];
    }
    new_path[size-1] = 0;
}
void print_path (char * path, char * str){
    system_call(SYS_WRITE, STDOUT, path, simple_strlen(path));
    system_call (SYS_WRITE, STDOUT, "/", 1);
    system_call(SYS_WRITE, STDOUT, str, simple_strlen(str));
    system_call (SYS_WRITE, STDOUT, "\n", 1); 
}
void build_file_not_exist (char * name, char * str){
    int name_len = simple_strlen(name);
    char * first = "The file '";
    char * second = "' Does not exist.";
    int first_len = 10, second_len = 17;
    int i,j;
    for (i=0; i<first_len; i++){
        str[i] = first[i];
    }
    for (j=0; j<name_len; j++){
        str[i] = name[j];
        i++;
    }
    for (j=0; j<second_len; j++){
        str[i] = second[j];
        i++;
    }
    str[i++] = '\n';
    str[i] = 0;
}
void create_command(char * command, char * path, char * str){
    int i, command_len = simple_strlen(command), path_len = simple_strlen(path);
    for (i=0; i<command_len; i++){
        str[i] = command[i];
    }
    int j;
    str[i++]=' ';
    for (j=0; j<path_len; j++){
        str[i] = path[j];
        i++;
    }
    str[i] = 0;
}
int found = 0;
int recursive_print_dir (char * path, char option ,char * file_name, char * func){
    if (found){
        return 0;
    }
    char buffer[2048];
    int buff = 2048;
    int file_descriptor = system_call(SYS_OPEN, path, 0x000, 0x00777); /* read only, and permission for all the users */
    if (file_descriptor < 0){
        system_call(SYS_EXIT, 0x55);
    }
    while (1){
        int read = system_call(SYS_GETDENTS, file_descriptor, buffer, buff);
        if (read == -1){
            system_call(SYS_EXIT, 0x55);
        }
        if (read == 0){
            break;
        }
        int bpos;
        for (bpos = 0; bpos < read;) {
            struct linux_dirent * d =(struct linux_dirent *)(buffer + bpos);
            char d_type = *(buffer + bpos + d->d_reclen - 1);
            char * str = d->d_name;
            bpos += d->d_reclen;
            if (simple_strncmp(str, ".",1)==0){
                continue;
            }
            if (option=='a' || (option=='n' && simple_strncmp(file_name, str, simple_strlen(str))==0)){
                print_path(path,str);
            }
            if (option=='e' && (simple_strncmp(file_name, str, simple_strlen(str))==0) && !found){
                int old = simple_strlen(path), add = simple_strlen(str);
                int size = 2 + old + add;
                char new_path[size];
                create_new_path(path, str, new_path);
                int new_size = size + 1 + simple_strlen(func); /* +1 for space */
                char call[new_size];
                create_command (func, new_path, call);
                simple_system(call);
                system_call(SYS_CLOSE, file_descriptor);
                system_call(SYS_CHROOT, 0);
                system_call(SYS_WRITE, STDOUT, "\n", 1);
                found = 1;
                return 0;
            }
            if (d_type == 4 &&!found){
                int old = simple_strlen(path), add = simple_strlen(str);
                int size = 2 + old + add;
                char new_path[size];
                create_new_path(path, str, new_path);
                recursive_print_dir(new_path, option, file_name, func);
            }
        }
    }
    if (option=='e'&&!found){
        int str_size = 29 + simple_strlen(file_name);
        char error[str_size];
        build_file_not_exist(file_name, error);
        system_call(SYS_WRITE, STDOUT, error, str_size);
        found = 1;
    }
    system_call(SYS_CLOSE, file_descriptor);
    return 0;
}
int main (int argc , char* argv[], char* envp[])
{
    switch (argc){
        case 1:
            recursive_print_dir(".", 'a',"", "");
            break;
        case 3:
            recursive_print_dir(".", 'n',argv[2], "");
            break;
        case 4:
            recursive_print_dir(".", 'e',argv[2],argv[3]);
            break;
        default:
            system_call(SYS_EXIT, 0x55);
    }
  return 0;
}
