/* 标准库头文件 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 操作系统头文件 */
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

/* 其他头文件 */
#include "passwd.h"

#define BUFFSIZE 512
#define BACKLOG 32
const static int port = 9901;

void deal_msg(const char *buf, char *response)
{
    char key[48];
    int i = 0, index = 0;
    for (; index < BUFFSIZE && buf[index] != ':'; ){
        key[i++] = buf[index++];
    }
    key[i] = '\0';
    index++;
    /* login */
    if (!strcmp(key, "login")){
        char name[48], passwd[48];
        for(i = 0; index < BUFFSIZE && buf[index] != ':';){
            name[i++] = buf[index++];
        }
        name[i] = '\0';
        index++;
        /* passwd以'\0'结尾 */
        for(i = 0; index < BUFFSIZE && buf[index] != '\0';){
            passwd[i++] = buf[index++];
        }
        passwd[i] = '\0';
        if (check_passwd(name, passwd) == 0) {
            strcat(response, "login:success");
        } else {
            strcat(response, "login:fail");
        }
    } else {
        strcat(response, "unrecognized message");
    }
}

/* 接受客户端请求 */
void *accept_request(void* p_sockfd)
{
    int client_socket = *(int *)p_sockfd;
    char msg[BUFFSIZE];
    // 不断读写
    while (1) {
        memset(msg, 0, sizeof(msg));
        int size = read(client_socket, msg, sizeof(msg));
        if (size  < 0){
            puts("recv error");
            break;
        } else if (size == 0) {
            break;
        }
        printf("msg : %s \n", msg);
        char response[BUFFSIZE];
        memset(response, 0, sizeof(response));
        deal_msg(msg, response);
        printf("res : %s \n", response);
        if (write(client_socket, response, sizeof(response)) < 0) {
            puts("send error");
            break;
        }
    }
}

/* 创建socket，创建线程处理客户端请求 */
int main()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        puts("socket error");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        puts("bind error");
        exit(1);
    }

    if (listen(server_socket, BACKLOG) < 0) {
        puts("listen error");
        exit(1);
    }

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL); // 空指针表示不关心客户端标志
        if (client_socket < 0) {
            puts("accept error");
            continue;
        }
        pthread_t newthread;
        if (pthread_create(&newthread , NULL, accept_request, &client_socket) != 0) {
            puts("pthread_create failed");
        }
    }

    close(server_socket);

    return 0;
}
