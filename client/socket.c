#include "socket.h"

// 和服务器建立TCP连接，返回sockfd
int connect_server(char *ip, int port)
{
	int sockfd = socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("socket error");
		exit(1);
	}
	
	//往serveraddr填入ip、port、地址族类型（IPv4）
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	//将ip地址转化为网络字节序
	//inet_pton(AF_INET,"127.0.0.1",&serveraddr.sin_addr.s_addr);
	serveraddr.sin_addr.s_addr=inet_addr(ip);

	/*步骤2：客户端调用connect函数连接到服务器端
	*/
	int res;
	if ((res = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) < 0){
		perror("connect error");
		exit(1);
	}
    return sockfd;
}

int send_message(int sockfd, char *msg, size_t size)
{
    if (write(sockfd, msg, size) < 0){
		perror("write msg error");
	}
}

int recv_message(int sockfd, char *msg, size_t size)
{
    if (read(sockfd, msg, size) < 0){
		perror("read msg error");
	}
}