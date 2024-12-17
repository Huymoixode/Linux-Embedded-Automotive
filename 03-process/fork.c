#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


void func(int signum){
    printf("This is func()\n");
    wait(NULL);
}

int main(){
    pid_t child = fork();
    if(child == 0){
        printf("This is child process PID %d\n",getpid());
        sleep(10);
    }else if(child > 0){
        printf("this is parent PID = %d\n",getpid());
        signal(SIGCHLD,func);
        printf("DONE child process \n");
        while(1);
    }else{
        printf("fork() fail!\n");
    }
}