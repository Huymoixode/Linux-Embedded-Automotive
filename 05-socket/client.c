#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>
#include <sys/socket.h>
#include <netdb.h>
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
#define MAX_CLIENT 10  // max client can connected on time
struct pollfd fds[MAX_CLIENT];
struct sockaddr_in conection_addr[MAX_CLIENT];
int client_count = 1;

void disconect_client(int index){
    close(fds[index].fd);
    for(int i= index; i < client_count - 1;i++){
        fds[i].fd = fds[i+1].fd;
       conection_addr[i] = conection_addr[i+1];
    }
    client_count--;
    fds[client_count].fd = -1;
    printf("Client %d disconected\n",index);
}

void *listen_conection(void *arg){
    while(1){

        //printf("LOCK\n");
        int ret = poll(fds,client_count,200); // return num of fd events
        if(ret == -1){
            // printf("fail to poll\n");
            continue;
        }else if(ret == 0){
            // printf("UNLOCK\n");
            continue;
        }
        //printf("Start checking and lock mutex numclient %d\n",client_count);
        pthread_mutex_lock(&mtx);
        for(int i=0;i < client_count;i++){
           // printf("scan %d client %d\n ",i,client_count);
            if(fds[i].revents & POLLIN){
                //printf("Checking %d\n",i);
                if(fds[i].fd == fds[0].fd){ // check if server received connection
                    socklen_t addr_len = sizeof(conection_addr[client_count]);
                    int new_fd = accept(fds[0].fd,(struct sockaddr *)&conection_addr[client_count],&addr_len);
                    if(new_fd == -1){
                        printf("Conected fail \n");
                        continue;
                    }else{
                        printf("New connection IP %s and port %d\n",inet_ntoa(conection_addr[client_count].sin_addr),ntohs(conection_addr[client_count].sin_port));
                        if(fds[client_count].fd <=0){

                            fds[client_count].fd = new_fd;
                            fds[client_count].events = POLLIN;
                            client_count++;
                            pthread_mutex_unlock(&mtx);
                        }
                    }
                }else{ //received data
                    char buff[128];
                    int bytes = recv(fds[i].fd,buff,sizeof(buff),0);
                    if(bytes == 0){
                        disconect_client(i);
                    }else if(bytes >0){
                        printf("Received message from %d : %s",i,buff);
                    }

                }

            }
        }
        //duyet xong so luong client
        pthread_mutex_unlock(&mtx);
    }
}
void connect_client(char ip[],int port){
    pthread_mutex_lock(&mtx);
    fds[client_count].fd = socket(AF_INET,SOCK_STREAM,0);
    if(fds[client_count].fd == -1){
        printf("fail to creat socket\n");
        close(fds[client_count].fd);
        exit(EXIT_FAILURE);
    }
    // thêm kết nối mới
    conection_addr[client_count].sin_family = AF_INET;
    conection_addr[client_count].sin_addr.s_addr = inet_addr(ip);
    conection_addr[client_count].sin_port = htons(port);

    if(connect(fds[client_count].fd,(struct sockaddr *)&conection_addr[client_count],sizeof(conection_addr[client_count])) == -1){
        printf("fail to connection\n");
        pthread_mutex_unlock(&mtx);
        exit(EXIT_FAILURE);
    }
    fds[client_count].events = POLLIN;
    printf("%d %s %d\n",client_count,inet_ntoa(conection_addr[client_count].sin_addr),ntohs(conection_addr[client_count].sin_port));
    client_count++;
    pthread_mutex_unlock(&mtx);
}

void send_message(char mess[],int index){
    ssize_t len = strlen(mess);
    int bytes = send(fds[index].fd,mess,len,0);
    printf("Send to connection id %d message: %s", index, mess);
}

void get_server_ip(int sockfd) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    
    if (getsockname(sockfd, (struct sockaddr *)&addr, &len) == -1) {
        perror("getsockname");
        return;
    }

    printf("Server's IP address: %s\n", inet_ntoa(addr.sin_addr));
}

void list_client(){
    printf("Total clients: %d\n", client_count - 1);
    printf("*******************************************\n");
    printf("ID |       IP Address    | Port No.\n");
    for(int i=1;i<client_count;i++){
        printf("%d  |    %s  | %d\n",i,inet_ntoa(conection_addr[i].sin_addr),ntohs(conection_addr[i].sin_port));
    }
    printf("*******************************************\n");
}

int create_Socket_Server(struct pollfd *fds,int port_no){
    fds->fd = socket(AF_INET,SOCK_STREAM,0);
    if (fds->fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    conection_addr[0].sin_family = AF_INET;
    conection_addr[0].sin_port = htons(port_no);
    conection_addr[0].sin_addr.s_addr =INADDR_ANY; //inet_addr("192.168.111.128"); 
    if(bind(fds->fd,(struct sockaddr *)&conection_addr[0],sizeof(conection_addr[0])) == -1){
        printf("fail to bind\n");
        close(fds->fd);
        return -1;
    }
    if(listen(fds->fd,MAX_CLIENT-1) == -1){
        printf("cannot listen on port no\n");
        close(fds->fd);
        return -1;
    }
    fds->events = POLLIN;
    
    return 0;
}

void print_menu() {
  printf("\n**************** Chat Application ****************\n");
  printf("Use the commands below:\n");
  printf("1. help\t\t: display user interface options\n");
  printf("2. myip\t\t: display IP address of this app\n");
  printf("3. myport\t\t: display listening port of this app\n");
  printf("4. connect <destination> <port no>\t: connect to the app of another computer\n");
  printf("5. list\t\t: list all the connections of this app\n");
  printf("6. terminate <connection id>\t: terminate a connection\n");
  printf("7. send <connection id> <message>\t: send a message to a connection\n");
  printf("8. exit\t\t: close all connections & terminate this app\n");
  printf("**************************************************\n");
}

int main(int argc, char *argv[]){
    char command[50];
    if(argc !=2){
        printf("Input port number to start\n");
        return 1;
    }
    
    pthread_t thread1;
    int port_no = atoi(argv[1]);
    if(create_Socket_Server(&fds[0],port_no) == -1){
        printf("Fail to create Server socket\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&thread1, NULL, listen_conection, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    while(1){
        char require[10];
        char argu1[20];
        char argu2[20];
        printf("PLEASE INPUT COMMAND:");
        fgets(command,sizeof(command),stdin);
        command[strcspn(command,"\n")] = '\0';
        sscanf(command,"%s %s %s",require,argu1,argu2);
        if(strcmp(require,"connect") == 0){
            int t = atoi(argu2);
            connect_client(argu1,t);
        }else if(strcmp(require,"list") == 0){
            list_client();
        }else if(strcmp(require,"terminate") == 0){
            int t =atoi(argu1);
            disconect_client(t);
        }else if(strcmp(require,"send") == 0){
            int t = atoi(argu1);
            send_message(argu2,t);
        }else if(strcmp(require,"myip") == 0){
            get_server_ip(fds[0].fd);
        }else if(strcmp(require,"help") == 0){
            print_menu();
        }
        // if(strcmp(require,"con") == 0){
        //     int port = atoi(argu2);
        //     connect_client(argu1,port);
        // }
    }
}
