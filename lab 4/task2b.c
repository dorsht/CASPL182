#include "lab4_util.h"

#define SYS_READ 0
#define SYS_WRITE 1
#define STDOUT 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_EXIT 60
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
int recursive_print_dir (char * path, short with ,char * file_name){
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
            if (!with || simple_strncmp(file_name, str, simple_strlen(str))==0){
                print_path(path, str);
            }
            if (d_type == 4){
                int old = simple_strlen(path), add = simple_strlen(str);
                int size = 2 + old + add;
                char new_path[size];
                create_new_path(path, str, new_path);
                recursive_print_dir(new_path, with, file_name);
            }
        }
    }
    
    system_call(SYS_CLOSE, file_descriptor);
    return 0;
}
int main (int argc , char* argv[], char* envp[])
{
    if (argc==3){
        if (simple_strncmp("-n", argv[1], 2)==0){
            recursive_print_dir(".", 1,argv[2]);
        }
    }
    else{
        recursive_print_dir(".", 0,"");
    }
  return 0;
}
