#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "passwd.h"

#define BUFFSIZE 512
static int server_socket;
const static int port = 9901;

void deal_msg(char *buf, char *response)
{
    char key[48];
    int i = 0, index = 0;
    for (; index < BUFFSIZE && buf[index] != ':'; ){
        key[i++] = buf[index++];
    }
    key[i] = '\0';
    index++;
    // login
    if (!strcmp(key, "login")){
        char name[48], passwd[48];
        for(i = 0; index < BUFFSIZE && buf[index] != ':';){
            name[i++] = buf[index++];
        }
        name[i] = '\0';
        index++;
        // passwd以'\0'结尾
        for(i = 0; index < BUFFSIZE && buf[index] != '\0';){
            passwd[i++] = buf[index++];
        }
        passwd[i] = '\0';
        printf("name : %s \npasswd : %s\n", name, passwd);
        if (check_passwd(name, passwd)){ // 用passwd模块查询
            strcat(response, "login:fail");
        }
        else {
            strcat(response, "login:success");
        }
    }
    else if (!strcmp(key, "display")){
        // 测试用
        strcpy(response, "status:success;data:abc:1;aaa:1;bbb:0");
    }
}

int main()
{
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("bind error");
        exit(1);
    }
    if(listen(server_socket, 48) < 0){
        perror("listen error");
        exit(1);
    }

    struct sockaddr_in client_addr;
    int lSize;
    while (1){
        lSize = sizeof(client_addr);
        memset(&client_addr, 0, sizeof(client_addr));
        // clientsocket是客户端的套接字
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &lSize);
        if (client_socket < 0){ // 连接出错
            perror("socket error");
            break;
        }
        char buf[BUFFSIZE];
        while(1){
            int size;//若接收成功，size记录接受的字节数
            memset(buf, 0, sizeof(buf));
            if ((size = recv(client_socket, buf, sizeof(buf), 0)) < 0){
                perror("recv error");
                break;
            } else if (size == 0){ //客户端在断开连接
                break;
            } else {
                // buf为接收的报文, response为应答报文
                char response[BUFFSIZE];
                memset(response, 0, BUFFSIZE);
                deal_msg(buf, response);
                if (send(client_socket, response, sizeof(response), 0) < 0){
                    if (errno==EPIPE){
                        // 当客户段断开连接，而服务器想写时，发出这个信号
                        break;
                    }
                    perror("send error");
                }
            }
        }
    }
    return 0;
}
