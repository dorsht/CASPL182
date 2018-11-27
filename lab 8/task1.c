#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
int fd;
void *map_start; /* will point to the start of the memory mapped file */
struct stat fd_stat; /* this is needed to  the size of the file */
Elf64_Ehdr *header; /* this will point to the header structure */
int num_of_section_headers;
char * str_table;
void clean_helper(){
    if (fd!=-1){
        // close the file using mmap functions
        munmap(map_start, fd_stat.st_size);
        close(fd);
    }
}
void quit(){
    clean_helper();
    exit(0);
}
void examine_ELF_file(){
    clean_helper();
    printf("Enter file name: ");
    char file_name[100];
    fgets(file_name,100,stdin);
    char path[2048];
    getcwd(path,2048);
    strcat(path,"/");
    strncat(path, file_name, strlen(file_name)-1);
    if((fd = open(path, O_RDWR)) < 0 ) {
        perror("error in open");
    }
    else if(fstat(fd, &fd_stat) != 0 ) {
        perror("stat failed");
    }
    else if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
    }
    else{
        header = (Elf64_Ehdr*)map_start;
        // Bytes 1,2,3 of the magic number
        printf("Magic numbers: %x %x %x\n",(header->e_ident)[0],(header->e_ident)[1],(header->e_ident)[2]);
        printf("Data: ");
        switch (header->e_ident[EI_DATA]){
            case ELFDATANONE:
                printf("Unknown data format\n");
                break;
            case ELFDATA2LSB:
                printf("2's complement, little endian\n");
                break;
            case ELFDATA2MSB:
                printf("2's complement, big endian\n");
                break;
        }
        printf("The Entry Point is: 0x%lx\n", header->e_entry);
        //  The file offset in which the section header table resides. 
        printf("The file offset of section header: %ld (bytes into file)\n", header->e_shoff);
        //  The number of section header entries. 
        printf("Number of section headers: %d\n", header->e_shnum);
        // The size of each section header entry. 
        printf("Size of section headers: %d (bytes)\n", header->e_shentsize);
        //  The file offset in which the program header table resides. 
        printf("The file offset of section program: %ld (bytes into file)\n", header->e_phoff);
        //  The number of program header entries. 
        printf("Number of program headers: %d\n", header->e_phnum);
        //  The size of each program header entry. 
        printf("Size of program headers: %d (bytes)\n",header->e_phentsize);
    }
}
char* get_name(int id){
    if (id>0){
        return (str_table+id);
    }
    return "NULL";
}
char* get_type (unsigned int type){
    return (type==SHT_NULL) ? "NULL" :
    (type == SHT_PROGBITS) ? "PROGBITS" :
    (type == SHT_SYMTAB) ? "SYMTAB" :
    (type == SHT_STRTAB) ? "STRTAB" :
    (type == SHT_RELA) ? "RELA" :
    (type == SHT_HASH) ? "HASH" :
    (type == SHT_DYNAMIC) ? "DYNAMIC" :
    (type == SHT_NOTE) ? "NOTE":
    (type==SHT_NOBITS) ? "NOBITS":
    (type==SHT_REL) ? "REL":
    (type==SHT_SHLIB) ? "SHLIB":
    (type == SHT_DYNSYM) ? "DYNSYM":
    (type == SHT_LOPROC) ? "LOPROC":
    (type == SHT_HIPROC) ? "HIPROC":
    (type==SHT_LOUSER) ? "LOUSER":
    (type == SHT_HIUSER) ? "HIUSER":
    "";
}
void print_section_names(){
    if (fd!=-1 && header!=NULL){
        int i;
        int offset = header->e_shoff;
        int next = 0;
        Elf64_Shdr * section_header = NULL;
        str_table = (char *)(map_start+((Elf64_Shdr *)(map_start+offset+(sizeof(Elf64_Shdr)*((Elf64_Ehdr *)map_start)->e_shstrndx)))->sh_offset);
        printf("%-7s %-20s %-15s %-10s %-10s %-10s \n",  "[Num]","Name", "Address", "Offset", "Size","Type");
        for (i=0; i<header->e_shnum; i++){ 
            printf("[%3d]%-3s",i," ");
            section_header = (Elf64_Shdr*)(map_start+next+offset);
            char * name = get_name(section_header->sh_name);
            printf("%-20s ",name);
            printf("%-15lx ",section_header->sh_addr);
            printf("%-10lx ",section_header->sh_offset);
            printf("%-10lx ",section_header->sh_size);
            char * type = get_type(section_header->sh_type);
            if (strcmp(type,"")==0){
                char temp[strlen(type)-1];
                int j;
                for (j=0; j<strlen(type); j++){
                    if (type[j+1]>='a' && type[j+1]<='z'){
                        temp[j] = type[j+1]-32; // convert to big letters
                    }
                    else{
                        temp[j] = type[j+1];
                    }
                }
                printf("%-10s ",temp);
            }
            else{
                printf("%-10s",type);
            }
            next+=sizeof(Elf64_Shdr);
            printf("\n");
        }
    }
    else{
        printf("File not initiallized\n");
    }
}
void print_selects(){
    printf("Choose Action:\n");
    printf("1 - Examine ELF File\n");
    printf("2 - Print Section Names\n");
    printf("3 - Quit\n");
}
typedef void (*myELF_func)();
myELF_func arr[3] = {examine_ELF_file, print_section_names,quit};
int main(int argc, char **argv) {
    fd = -1;
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
               (arr[select-1]());
               break;
           default:
                printf("Illegal Selection, try again\n");
                break;
       }
    }
}