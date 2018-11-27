#include "lab4_util.h"

#define SYS_WRITE 1
#define STDOUT 1
#define SYS_READ 0
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_LSEEK 8
#define SYS_EXIT 60
extern int system_call();

int main (int argc , char* argv[], char* envp[])
{
    if (argc!=3){
        system_call(SYS_EXIT,0x55);
    }
    int file_desc = system_call(SYS_OPEN, argv[1] ,0x002 ,0777);
    if (file_desc<0)
    {
        system_call(SYS_CLOSE, file_desc);
        system_call(SYS_EXIT,0x55);
    }
    int offset = system_call(SYS_LSEEK, file_desc, 4117, 0);/* 4117 (decimal) == 1015 (hex), found with hexedit */
    if (offset<0){
        system_call(SYS_CLOSE, file_desc);
        system_call(SYS_EXIT,0x55);
    }
    
    char str[8];
    int i, len = simple_strlen(argv[2]);
    for (i=0; i<len; i++){
        str[i] = argv[2][i];
    }
    str[i++] = '.';
    while (i<7){
        str[i++] = ' ';
    }
    str[i++] = '\n';
    system_call(SYS_WRITE, file_desc, str, 8);
    system_call(SYS_CLOSE, file_desc);
  return 0;
}