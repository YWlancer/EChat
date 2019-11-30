#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string.h> // for size_t

/* 和服务器建立TCP连接，成功返回sockfd，失败返回-1 */
int connect_server(const char *ip, int port);

/* 发送数据，若出错，返回-1 */
int send_message(int sockfd, const void *msg, size_t size);

/* 接受数据，若出错，返回-1 */
int recv_message(int sockfd, void *msg, size_t size);

#endif
