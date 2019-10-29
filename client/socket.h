#ifndef SOCKET_H_
#define SOCKET_H_

int connect_server(char *ip, int port);
int send_message(int sockfd, char *msg, size_t size);
int recv_message(int sockfd, char *msg, size_t size);

#endif // SOCKET_H_
