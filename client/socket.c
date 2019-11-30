#include "socket.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* 和服务器建立TCP连接，成功返回sockfd，失败返回-1 */
int connect_server(const char *ip, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        return -1;
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        return -1;
    }
    return sockfd;
}

/* 发送数据，若出错，返回-1 */
int send_message(int sockfd, const void *msg, size_t size)
{
    int ret = -1;
    if (write(sockfd, msg, size) != size){
        return ret;
    }
    return size;
}

/* 接受数据，若出错，返回-1 */
int recv_message(int sockfd, void *msg, size_t size)
{
    int ret = -1;
    ret = read(sockfd, msg, size);
    return ret;
}
