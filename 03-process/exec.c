#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int task(){
    return 32;
}
int main(int argc, char* argv[]){
    pid_t pid = fork();
    if(pid == 0){
        printf("The child process PID = %d\n",getpid());
        int res = task();
        while(1); // đặt vòng lặp để test sigkill . Nếu bị kill = -15(term ) hoặc -9 thì sẽ không bh gọi exit()
       // exit(res);
    }else if(pid > 0){
        printf("This is parent process PID = %d and child process PID = %d\n",getpid(),pid);
        int status;
        pid_t child_pid = wait(&status);
        if(WIFEXITED(status)){ // nếu tiến trình hoạt động bình thường và thoát tự nhiên
            int exit_code = WEXITSTATUS(status);
            printf("Child process %d returned %d\n",child_pid,exit_code);  
        }else if(WIFSIGNALED(status)){ // if child process kill by sigterm or sigkill
            printf("Child process was signaled code %d\n",WTERMSIG(status));
        }else if(WIFSTOPPED(status)){  // child process was stopped by stopsig 
            printf("Child process was stopped code %d\n",WSTOPSIG(status));
        }
/* Nếu tiến trình cha bị stop và tiến trình con bị kill bởi sigal. Tiến trình con sẽ vào trạng thái Zombie*/
/*Để tiếp tục có thể gọi sigcont(-18) cho tiến trình cha tiếp tục gọi wait()*/
    }else{
        return 1;
    }
    return 0;
}