#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc,char *argv[]){
	/*步骤1：创建socket
	*/
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		perror("socket error");
		exit(1);
	}
	
	//往serveraddr填入ip、port、地址族类型（IPv4）
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi("9901"));
	//将ip地址转化为网络字节序
	//inet_pton(AF_INET,"127.0.0.1",&serveraddr.sin_addr.s_addr);
	serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	/*步骤2：客户端调用connect函数连接到服务器端
	*/
	int res;
	if ((res = connect(sockfd, (struct sockaddr*)&serveraddr,
		sizeof(serveraddr)))<0){
		perror("connect error");
		exit(1);
	}
	/*步骤3：调用IO函数（read/write）和服务器
	 *进行双向通信
	*/
	char buff[1024];
	size_t size;
	char *prompt=">";
	while(1){
		memset(buff,0,sizeof(buff));
		write(STDOUT_FILENO,prompt,1);					//打印到屏幕
		size=read(STDIN_FILENO,buff,sizeof(buff));		//输入字符串，read并不会将最后一位设置为'\0'
		
		char endChar[]="close socket";					//当输入close socket的时候退出循环
		if(size==sizeof(endChar) && !strncmp(buff,endChar,strlen(endChar))){
			break;
		}

		if(size<0)
			continue;
		buff[size-1]='\0';								//将数组最后一位作为结束符
		if(write(sockfd,buff,sizeof(buff))<0){
			perror("write msg error");					//输入出错
			continue;
		}else{											//输入成功，接受服务器端数据
			if(read(sockfd,buff,sizeof(buff))<0){
				perror("read msg error");				//读取出错
				continue;
			}else{
				printf("%s\n",buff);					//读取成功
			}
		}
	}

	/*步骤4：关闭socket套接字
	*/
	close(sockfd);

	return 0;
}
