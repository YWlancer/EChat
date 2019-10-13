#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <arpa/inet.h>

int connect_server(char *ip, int port);
int send_message(int sockfd, char *msg, size_t size);
int recv_message(int sockfd, char *msg, size_t size);