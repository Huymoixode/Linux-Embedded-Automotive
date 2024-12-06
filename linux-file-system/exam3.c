#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#define BUFF_SIZE 128
void main(int argc, char* argv[]){
    char buffer[BUFF_SIZE];
    if(argc !=5 || strcmp(argv[1],"--help")==0){
        printf("$ ./example_program filename num-bytes [r/w] 'Hello' ");
        return ;
    }
    const char* filename = argv[1];
    ssize_t numbyte = atoi(argv[2]);
    char *buff = argv[4];

    int fd = open(filename,O_CREAT | O_RDWR,0664);
    if(fd < 0){
        printf("Cannot open file direction\n");
        return;
    }
    switch (argv[3][0])
    {
    case 'r':
        if(read(fd,buffer,numbyte)){
            printf("buffer: %s",buffer);
        }else{
            printf("Fail to read file\n");
        }
        break;
    case 'w':
        if(write(fd,buff,numbyte)){
            printf("Write Successfully");
        }else{
            printf("Fail to write file\n");
        }
        break;
    }
    close(fd);
}