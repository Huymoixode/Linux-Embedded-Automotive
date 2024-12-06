#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define BUF_SIZE 128

void write_to_file(const char *filename,const char *data){
    int fd = open(filename,O_WRONLY | O_TRUNC | O_CREAT,0664);
    if(fd == -1 ){
        printf("fail to open file \n");
        return; 
    }
    ssize_t size;
    size = write(fd, data,strlen(data));
    if(size != strlen(data)){
        printf("Fail to write\n");
    }
    close(fd);
}
int main(int argc, char* argv[]){
    //const char *filename = "example.txt";
    const char *data = "Hello World \n";
    const char *append_data = "Append Data\n";
    if(argc != 2 || strcmp(argv[1],"--help") == 0){
        printf("./file filename\n");
        return 1;
    }

    write_to_file(argv[1],data);
    // read_from_file(filename);
    // append_to_file(filename,append_data);
    // seek_and_read(filename,7);
}