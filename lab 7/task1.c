#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

char file_name[100];
int * size;
void quit(){
    if (size!=NULL){
        free(size);
    }
    exit(0);
}
void set_file_name(){
    printf("Enter file name: ");
    fgets(file_name,100,stdin);
}
void set_unit_size(){
    char input[100];
    printf("Enter file size: ");
    fgets(input,100,stdin);
    int temp = atoi(input);
    switch (temp){
        case 1:
        case 2:
        case 4:
            *size = temp;
            break;
        default:
            printf("Illegal unit size\n");
            break;
    }
}
void file_display(){
    if (file_name==NULL){
        printf("Error: Can't find file null...\n");
        return;
    }
    char path[2048];
    getcwd(path,2048);
    strcat(path,"/");
    strncat(path, file_name, strlen(file_name)-1);
    int fd = open(path, O_RDONLY,0777); // all options, path is the path for the file...
    if (fd == -1){
        printf("Error: File not exist!\n");
        return;
    }
    if (*size==0){
        printf("Unit size isn't initiallized\n");
        return;
    }
    else{
        printf("Please enter <location> <length>\n");
        int * location = calloc(1,sizeof(int));
        int * length = calloc(1,sizeof(int));
        scanf("%x %d",location,length);
        while(getchar()!='\n');
        if (*location<0 || *length<0){
            printf("Error: Illegal location or length!\n");
        }
        else{
            int buf = (*size)*(*length);
            unsigned int * data = calloc(buf,sizeof(int));
            lseek(fd, *location , SEEK_CUR);
            read(fd, data, buf);
            close(fd);
            printf("Hexadecimal Representation:\n");
            int i;
            unsigned char * pointer = (unsigned char*)data;
            for (i=0; i<*length; i++){
                switch(*size){
                    case 1:
                        printf("%02x", pointer[i]);
                        break;
                    case 2:
                        printf("%04x",((unsigned short*)pointer)[i]);
                        break;
                    case 4:
                        printf("%08x",((unsigned int*)pointer)[i]);
                        break;
                    default:
                        break;
                }
                printf(" ");
            }    
            printf("\nDecimal Representation:\n");
            for (i=0; i<buf; i=i+2){
                printf("%d ", (unsigned int)pointer[i]|(unsigned int)pointer[i+1]<<8);
            }
            printf("\n");
            free(data);
        }
        free(location);
        free(length);
    }
}
void file_modify(){
    if (file_name==NULL){
        printf("Error: Can't find file null...\n");
        return;
    }
    char path[2048];
    getcwd(path,2048);
    strcat(path,"/");
    strncat(path, file_name, strlen(file_name)-1);
    int fd = open(path, O_WRONLY,0777); // all options, path is the path for the file...
    if (fd == -1){
        printf("Error: File not exist!\n");
        return;
    }
    if (*size==0){
        printf("Unit size isn't initiallized\n");
        return;
    }
    else{
        printf("Please enter <location> <val>\n");
        int * location = calloc(1,sizeof(int));
        int * val = calloc(1,sizeof(int));
        scanf("%x %x",location,val);
        while(getchar()!='\n');
        if (*location<0){
            printf("Error: Illegal location\n");
        }
        else{
            int buf = (*size);
            lseek(fd, *location , SEEK_CUR);
            write(fd,val,buf);
            close(fd);
        }
        free(location);
        free(val);
    }
}
void copy_from_file(){
    // assume that there's dest_file
    if (file_name==NULL){
        printf("Error: Can't find file null...\n");
        return;
    }
    int * src_offset = calloc(1,sizeof(int));
    int * dst_offset = calloc (1,sizeof(int));
    int * length = calloc (1,sizeof(int));
    char src_file[100];
    printf("Please enter <src_file> <src_offset> <dst_offset> <length>\n");
    scanf("%s %x %x %d", src_file, src_offset, dst_offset, length);
    while(getchar()!='\n');
    if (*length<0 || *src_offset < 0 || *dst_offset < 0){
        printf("Error: Illegal argument\n");
        free(src_offset);
        free(dst_offset);
        free(length);
        return;
    }
    char path_src[2048];
    char path_dst[2048];
    getcwd(path_src,2048);
    getcwd(path_dst,2048);
    strcat(path_src,"/");
    strcat(path_dst,"/");
    strncat(path_dst, file_name, strlen(file_name)-1);
    strncat(path_src,src_file , strlen(src_file));
    int src_fd = open (path_src, O_RDONLY, 0777);
    if (src_fd==-1){
        printf("Error: Bad source file\n");
        free(src_offset);
        free(dst_offset);
        free(length);
        return;
    }
    int dst_fd = open (path_dst, O_RDWR, 0777);
    if (dst_fd == -1){
        close(src_fd);
        printf("Error: Bad destination file\n");
        free(src_offset);
        free(dst_offset);
        free(length);
        return;
    }
    char * data = calloc(*length, sizeof(char));
    lseek(src_fd, *src_offset, SEEK_CUR);
    read (src_fd, data, *length);
    lseek(dst_fd, *dst_offset, SEEK_CUR);
    write(dst_fd, data, *length);
    close(src_fd);
    close(dst_fd);
    printf("Loaded %d bytes into 0x%X FROM %s TO %s\n",*length, (*length)+(*dst_offset) ,src_file,file_name);
    free(src_offset);
    free(dst_offset);
    free(length);
    free(data);
}
void print_selects(){
    printf("1 - Set File Name\n");
    printf("2 - Set Unit Size\n");
    printf("3 - File Display\n");
    printf("4 - File Modify\n");
    printf("5 - Copy From File\n");
    printf("6 - Quit\n");
}
typedef void (*hex_edit_func)();
hex_edit_func arr[6] = {set_file_name, set_unit_size, file_display, file_modify,copy_from_file,quit};
int main(int argc, char **argv) {
   size = calloc(1,sizeof(int));
    printf("\n");
    while (1){
       print_selects();
       char input[100];
       fgets(input,100,stdin);
       int select = atoi(input);
       switch (select){
           case 1:
           case 2:
           case 3:
           case 4:
           case 5:
           case 6:
               (arr[select-1]());
               break;
           default:
                printf("Illegal Selection, try again\n");
                break;
       }
    }
}