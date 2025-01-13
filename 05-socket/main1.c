#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>
#include <poll.h>
#define SERVER_IP "192.168.111.128"
#define CLIENT_MAX 10
int client_count = 1;
int fd[CLIENT_MAX+1]; // fd[0] is server
char buff[1024];
struct sockaddr_in fd_addr,client_fd_addr[CLIENT_MAX];
struct pollfd fds[CLIENT_MAX];
int nfds = 1; // Ban đầu chỉ có server socket
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void clean_client(int index){
    pthread_mutex_lock(&mtx);
    close(fd[index]);
    fds[index].fd = -1;
    for(int i=index;i < nfds -1 ;i++){
        fd[i] = fd[i+1];
        printf("fd[%d]  = fd[%d]\n",i,i+1);
        fds[i] = fds[i+1];
        client_fd_addr[i-1] = client_fd_addr[i];
    }
    fd[nfds-1] = -1;
    fds[nfds-1].fd = -1;
    nfds--;
    pthread_mutex_unlock(&mtx);
    printf("Count khi down %d\n",nfds);
    //fflush(stdout);
}

// void *client_disconected(void *arg){
//     int byte_count;
//     while(1) {
//         pthread_mutex_lock(&mtx);
//         for (int i = 1; i < client_count; i++) {
//             byte_count = recv(fd[i], buff, sizeof(buff), MSG_DONTWAIT);
//             if (byte_count == 0) {
//                 // Client ngắt kết nối
//                 printf("Disconnected client %d %d\n", i,byte_count);
//                 pthread_mutex_unlock(&mtx);
//                 clean_client(i);
//                 i--; // Giảm chỉ số để kiểm tra client mới ở vị trí này
//             } else if (byte_count < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
//                 // Lỗi nghiêm trọng, xử lý nếu cần
//                 perror("recv error");
//             }else if(byte_count > 0){
//                 printf("Receive message from %d : %s\n",i,buff);
//             }
//         }
//         pthread_mutex_unlock(&mtx);
//     }
// }


// void *client_manager(void *arg) {
//     socklen_t *len = (socklen_t *)arg;
//     while (1) {
//         if (client_count > 5) {
//             printf("Max client\n");
//             usleep(1000); // Tránh tiêu thụ CPU 100%
//             continue;
//         }
//         int new_fd = accept(fd[0], (struct sockaddr *)&client_fd_addr[client_count - 1], len);
//         if (new_fd == -1) {
//             perror("client accept fail");
//         } else {
//             pthread_mutex_lock(&mtx);
//             fd[client_count] = new_fd;
//             client_count++;
//             pthread_mutex_unlock(&mtx);
//             printf("New client connected: %d\n", new_fd);
//         }
//     }
// }
void *poll_Clients(void *arg) {
    socklen_t *len = (void *)arg;
    while (1) {
        int ret = poll(fds, nfds, -1);  // Chờ sự kiện trên các FD không giới hạn thời gian
        if (ret < 0) {
            perror("poll failed");
            continue;
        }
        for (int i = 0; i < nfds; i++) {
            printf("%d\n",i);
            if (fds[i].revents & (POLLIN | POLLHUP | POLLERR | POLLOUT)) {
                if (fds[i].fd == fd[0]) {
                    // Xử lý kết nối mới
                    pthread_mutex_lock(&mtx);  // Khóa trước khi xử lý socket
                    int new_fd = accept(fd[0], (struct sockaddr *)&client_fd_addr[nfds - 1], &len[nfds - 1]);
                    if (new_fd < 0) {
                        perror("Fail to accept");
                        continue;
                    }
                    printf("New client %d\n", new_fd);
                    for (int j = 1; j < CLIENT_MAX; j++) {
                        if (fds[j].fd == -1) {
                            fd[j] = new_fd;
                            fds[j].fd = new_fd;
                            printf(" fd [%d] : %d \n",j,new_fd);
                            fds[j].events = POLLIN | POLLHUP |POLLERR |POLLOUT;
                            nfds++;
                            break;
                        }
                    }
                    pthread_mutex_unlock(&mtx);  // Mở khóa sau khi xử lý
                } else {
                    // Xử lý dữ liệu hoặc ngắt kết nối
                    int byte_count = recv(fd[i], buff, sizeof(buff),0);
                    if (byte_count == 0) {
                        printf("Client disconnected %d\n", i);
                        clean_client(i);
                        i--;
                        continue;
                    } else if (byte_count < 0) {
                        if (errno != EAGAIN && errno != EWOULDBLOCK) {
                            perror("recv error");
                            clean_client(i);
                            i--;
                            continue;
                        }
                    } else {
                        printf("Received message %s\n", buff);
                    }
                }
            }
        }
    }
}


void connection_sever(const char *ip, int port){
    struct sockaddr_in server_addr;
    pthread_mutex_lock(&mtx);
    fd[nfds] = socket(AF_INET,SOCK_STREAM,0);
    if(fd[nfds] == -1){
        printf("Cannot create socket \n");
        pthread_mutex_unlock(&mtx);
        exit(EXIT_FAILURE);
    }
    //setup server
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    //connecto to server
    if(connect(fd[nfds],(struct sockaddr *)&server_addr,sizeof(server_addr)) == -1){
        printf("connect to server fail \n");
        close(fd[nfds]);
        pthread_mutex_unlock(&mtx);
        exit(EXIT_FAILURE);
    }
    client_fd_addr[nfds-1] = server_addr;
    nfds++;
    pthread_mutex_unlock(&mtx);
    printf("Connected to server IP %s Port %d\n",inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));
}

void my_ip_server(){
    printf("My Ip: %s myPort: %d\n",inet_ntoa(fd_addr.sin_addr),ntohs(fd_addr.sin_port));
}

void send_Data(char *buff,int index){
    if(send(fd[index],buff,sizeof(buff),0) == -1){
        printf("fail to send data to %d\n",index);
    }
}
int main(int argc,char *argv[]){
    pthread_t thread1,thread2,thread3;
    char command[20];
    int port = atoi(argv[1]);
    socklen_t len_client[4];
    for(int i=0;i<CLIENT_MAX;i++){
        len_client[i] = sizeof(client_fd_addr[i]);
    }
    fd[0] = socket(AF_INET,SOCK_STREAM,0);
    fd_addr.sin_family = AF_INET;
    fd_addr.sin_port = htons(port);
    fd_addr.sin_addr.s_addr = inet_addr("192.168.111.128");
    if(bind(fd[0],(struct sockaddr *)&fd_addr,sizeof(fd_addr)) == -1){
        printf("blind fail\n");
        exit(EXIT_FAILURE);
    }
    if(listen(fd[0],5) == -1){
        printf("Listen fail");
        exit(EXIT_FAILURE);
    }
    printf("Listenning...\n");
    fds[0].events = POLLIN;
    for (int i = 1; i < 10; i++) {
        fds[i].fd = -1; // Đánh dấu vị trí trống
    }
    fds[0].fd = fd[0];
    // if(pthread_create(&thread1, NULL, client_manager, &len_client[0]) != 0) {
    //     perror("thread1 creation failed");
    //     return 1;
    // }
    // if(pthread_create(&thread2, NULL, client_disconected, &len_client[0]) != 0) {
    //     perror("thread2 creation failed");
    //     return 1;
    // }
    if(pthread_create(&thread3, NULL, poll_Clients, len_client) != 0) {
        perror("thread3 creation failed");
        return 1;
    }

    while(1){
        printf("Enter your command\n");
        fgets(command,sizeof(command),stdin);
        command[strcspn(command,"\n")] = '\0';
        if(strcmp(command,"list") == 0){
            printf("nfds %d\n",nfds);
            for(int i=1;i < nfds;i++){
                printf("%d ",fd[i]);
                if(fd[i] > 0){
                    printf("%d %s %d\n",i,inet_ntoa(client_fd_addr[i-1].sin_addr),ntohs(client_fd_addr[i-1].sin_port));
                }
            }
        }else if(strcmp(command,"connection") == 0){
            int port ; scanf("%d%*c",&port);
            
            connection_sever(SERVER_IP,port);
            for(int i= 0;i<5;i++){
                printf("%d ",fd[i]);
            }
        }else if(strcmp(command,"disconection") == 0){
            int index; scanf("%d%*c",&index);
            clean_client(index);
        }else if(strcmp(command,"myip") == 0){
            my_ip_server();
        }else if(strcmp(command,"send") == 0){
            char message[10] = "hello";
            int index;
            printf("Input ID want to send data: ");
            scanf("%d%*c",&index);
            printf("\nData to send:");
            fgets(message,sizeof(message),stdin);
            send_Data(message,index);
        }

    }
    return 0;
}   