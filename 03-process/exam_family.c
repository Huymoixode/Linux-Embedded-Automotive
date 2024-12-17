#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
void main(){
    char *argv[] = {"ls","-l",NULL};
    char *env[] = {"HELLO=hleleh",NULL};
    printf("Thực hiện lệnh exec ls -l");
    //execl("/bin/ls","ls","-l",NULL);
    //printf("dcasdaskdmaksd");
    // execv("/bin/ls",argv);
    // execlp("ls","ls","-l",NULL);
    // execvp("ls",argv);
    execle("/bin/ls","ls","-l",NULL,env);
}