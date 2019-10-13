#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<signal.h>
#include<errno.h>
#include<time.h>
#include<arpa/inet.h>


int sockfd;								//套接字描述符

void sig_handler(int signo){			//信号处理函数
	if(signo==SIGINT){
		printf("serve close\n");
		/*步骤6：关闭socket
		 *这里关闭服务器socket
		 */
		close(sockfd);
		exit(1);
	}
	if(signo==SIGCHLD){
		printf("child process deaded...");
		wait(0);
	}
}

void out_addr(struct sockaddr_in *clientaddr){	//输出连接上来的客户端相关信息
	//将端口从网络字节序转化为主机字节序
	int port=ntohs(clientaddr->sin_port);
	char ip[16];
	memset(ip,0,sizeof(ip));
	//将ip地址从网络字节序转化为点分十进制
	inet_ntop(AF_INET,&clientaddr->sin_addr.s_addr,ip,sizeof(ip));
	printf("client: %s(%d) connected\n",ip,port);
}
void do_service(int fd){						//和客户端进行双向通信
	/*和客户端进行读写操作（双向通信）*/
	char buff[1024];
	while(1){
		memset(buff,0,sizeof(buff));
		printf("start read and write...\n");
		size_t size;
		if((size==read(fd,buff,sizeof(buff)))<0){
			perror("protocal error");			//如果数据读取错误，则跳出循环
			break;
		}else if(size==0){
			/*可能有一种情况s，当客户端连接关闭
			 *即写端关闭，只留下读端,当缓冲区里
			 *的数据都被读完后，那么读取的大小就是0
			 *补充：若写端没有关闭，但是写端又不写入
			 *数据，那么读端就会阻塞。
			 */
			break;
		}else{									//正常读取
			printf("%s\n",buff);
			if(write(fd,buff,sizeof(buff))<0){
				if(errno==EPIPE){
					/*当服务端想向客户端写入数据时，若客户端断开连接
					 *就类似与管道的读端关闭，只留下写段，这样会产生
					 *一个SIGPIPE信号，同时errno会等于EPIPE
					 */
					break;
				}
				perror("protocal error");
			}
		}
	}
}

int main(int argc,char* argv[]){
	if(signal(SIGINT,sig_handler)==SIG_ERR){
		printf("signal sigint error");
		exit(1);
	}
	if(signal(SIGCHLD,sig_handler)==SIG_ERR){
		perror("signal sigchld error");
		exit(1);
	}

	/*步骤1：创建socket（套接字）
	 *注：socket创建在内核中，是一个结构体。
	 AF_INET:IPv4因特域
	 SOCK_STREAM：流式套接字，TCP协议
	 */	
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		perror("socket error");
		exit(1);
	}
	/*步骤2：调用bind函数将socket和地址
	 *（包括ip、port）进行绑定 
	 */
	struct sockaddr_in serveraddr;					//因特网专用地址
	memset(&serveraddr,0,sizeof(serveraddr));		//初始化一下
	//往地址中填入ip、port、internet地址族类型
	serveraddr.sin_family=AF_INET;					//IPv4
	serveraddr.sin_port=htons(atoi("9901"));		//端口，由命令行传入，需要用htons函数转成网络字节序
	//serveraddr.sin_addr.s_addr="192.168.0.10";	//ip地址这样设置的话，ip地址就绑死了
	serveraddr.sin_addr.s_addr=INADDR_ANY;			//监听服务器上所有网卡的ip
	
	if(bind(sockfd,(struct sockaddr*)&serveraddr,
	sizeof(serveraddr))<0){							//将套接字与地址绑定，地址需要强转成通用地址形式
		perror("bind error");
		exit(1);
	}

	/*步骤3：调用listen函数启用监听（指定port监听）
	 *通知系统去接管来自客户端的连接请求
	 *将接受到的客户端的连接请求放在对应的队列中
	 *listen的第二个参数是制定这个队列的大小
	 */
	if(listen(sockfd,10)<0){					
		perror("listen error");
		exit(1);
	}

	/*步骤4：调用acept函数从队列中获得一个客户端的请求连接
	 *并返回一个新的客户端描述符
	 *若没有客户端连接，则会阻塞，知道有一个客户端连接
	 */
	struct sockaddr_in clientaddr;					//用于保存客户端地址
	socklen_t clientaddr_len=sizeof(clientaddr);	//客户端地址长度
	while(1){
		int fd=accept(sockfd,(struct sockaddr*)&clientaddr,&clientaddr_len);
		if(fd<0){
			perror("accept error");
			continue;
		}
		
		/*步骤5：fork一个子进程
		 *调用IO函数（read/write）和连接的客户端
		 *进行双向通讯
		 */
		pid_t pid=fork();
		if(pid<0){
			continue;
		}else if(pid==0){							//子进程		
			out_addr(&clientaddr);
			do_service(fd);
			/*步骤6：关闭socket
			 *这里关闭客户端socket
		 	 */
			close(fd);
		}else{										//父进程
			/*当fork一个子进程的时候，子进程与父进程的环境相同
			 *因此，套接字的描述符fd也会复制一份，而内核对fd有
			 *一个类似于引用计数的东西，当父进程和子进程都close
			 *掉这个fd时，这个套接字才会真正关闭
			 */
			close(fd);
		}

	}


	return 0;
}

