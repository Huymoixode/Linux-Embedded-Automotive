#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


void callback(int signum){
    printf("Child process was received signal\n");
    exit(0);
}
void reschild(int signum){
    printf("Child process reverse successfully! \n");
    wait(NULL);
}
void main(){
    int pid =fork();
    if(pid == 0){
        printf("I'm child process PID: %d\n",getpid());
        signal(SIGUSR1,callback); // đăng kí nhận tín hiệu 
        while(1);
    }else if(pid > 0){
        printf("I'm parent process PPID:%d\n",getpid());
        signal(SIGCHLD,reschild);  // Nếu child process kết thúc
        sleep(3);
        kill(pid,SIGUSR1); // gửi tín hiệu đến child process 
        sleep(1);
    }else{
        printf("Fork() thất bại\n");
    }
}