#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "socket.h"

#define MAX_NAME 20
#define MAX_PASS 20
#define MAX_PS  100
#define MAX_LINE 100
#define BUFFSIZE 512
#define IP "127.0.0.1"
const static int port = 9901;
static int sockfd;

// prompt string，命令提示符
char PS[MAX_PS] = "";
char name[MAX_NAME + 1], passwd[MAX_PASS + 1];
char command[MAX_LINE];

int getname(char *, size_t);
int getpasswd(char *, size_t);
void check_args(int, char**);
void check_passwd();
int read_command();
void display_user(char *);
int command_parse();
void join_space(char *);

int main(int argc, char *argv[])
{
    check_args(argc, argv);
    if (!getname(name, sizeof(name)))
        exit(1);
    if (!getpasswd(passwd, sizeof(passwd)))
        exit(1);
    //建立连接，如果登录失败则断开连接
    sockfd = connect_server(IP, port);
    check_passwd();

    strcpy(PS, "EChat > ");
    while (printf("%s", PS) && read_command()){
        //解析命令
        if (!command_parse()){
            puts("error : unrecognizd command ");
            puts("input help for usage ");
        }
    }
    close(sockfd);
    exit(0);
}

// 出错返回0
int getname(char *name, size_t size)
{
    if (!strcmp(name, ""))
        return 1;//已通过命令行读入用户名
    printf("login:");
    scanf("%s", name);
    return 1;

}

// 出错返回0，以*回显密码
int getpasswd(char *pass, size_t size)
{
    printf("passwd:");
    scanf("%s", passwd);
    return 1;
}

// 提供注册 -r,直接输入用户名-u，-h
void check_args(int argc, char *argv[])
{
    if (argc == 1)
        return;
    else if (argc == 2 && strcmp(argv[1], "-h")){
        puts("usage : EChat [ options ] ");
        puts("options : ");
        puts("  -h               display this information ");
        puts("  -u name          use the name to log in ");
        puts("  -r               sign up for EChat ");
    }
    else if (argc == 3 && strcmp(argv[1], "-u")){
        strcpy(name, argv[2]);
    }
    else if (argc == 2 && strcmp(argv[1], "-r")){

    }
    else
        puts("error, input EChat -h to view usage ");
}

void check_passwd()
{
    char buf[BUFFSIZE] = "login:";
    strcat(buf, name);
    strcat(buf, ":");
    strcat(buf, passwd);
    send_message(sockfd, buf, sizeof(buf));
    char response[BUFFSIZE];
    recv_message(sockfd, response, sizeof(response));
    if (strcmp(response, "login:success")){
        printf("error : user doesn't exist or passwd is wrong.\n");
        close(sockfd);
        exit(0);
    }
    printf("login success : %s\n", name);
    // 清空密码
    memset(passwd, 0, sizeof(passwd));
}

// 按行读取命令
int read_command()
{
    gets(command);
    if (!strcmp(command, "q"))
        return 0;
    return 1;
}

//解析command，并执行,避免函数过长，可拆分
int command_parse()
{
    char str1[MAX_LINE];// 命令参数1
    int i = 0, j = 0;
    for(; i < MAX_LINE && isspace(command[i] && command[i]); ++i); // 跳过前面的空白字符
    for(; i < MAX_LINE && !isspace(command[i] && command[i]); ++i)
        str1[j++] = command[i];
    str1[j] = '\0'; j = 0;
    // display 命令
    if (!strcmp(str1, "display")){
        char str2[MAX_LINE]; //命令参数2
        for(; i < MAX_LINE && isspace(command[i] && command[i]); ++i); // 跳过前面的空白字符
        for(; i < MAX_LINE && !isspace(command[i] && command[i]); ++i)
            str2[j++] = command[i];
        str2[j] = '\0'; j = 0;
        if (strcmp(str2, "user")) //参数2错误
            return 0;
        char str3[MAX_LINE]; //命令参数3
        for(; i < MAX_LINE && isspace(command[i] && command[i]); ++i); // 跳过前面的空白字符
        for(; i < MAX_LINE && !isspace(command[i] && command[i]); ++i)
            str3[j] = command[i];
        str3[j] = '\0';
        if (!strcmp(str3, "all") || !strcmp(str3, "")){
            // display user all || display user
            char buf[BUFFSIZE] = "display:user:all";
            send_message(sockfd, buf, sizeof(buf));
            char response[BUFFSIZE];
            recv_message(sockfd, response, sizeof(response));
            // 打印接收到的数据
            display_user(response);
            return 1;
        }
        else if (!strcmp(str3, "online")){
            // display user online
            char buf[BUFFSIZE] = "display:user:online";
            send_message(sockfd, buf, sizeof(buf));
            char response[BUFFSIZE];
            recv_message(sockfd, response, sizeof(response));
            // 打印接收到的数据
            display_user(response);
            return 1;
        }
        else
            return 0; //参数3错误
    }

    // space 报文相关命令
    else if (!strcmp(str1, "create")){
        char str2[MAX_LINE]; //命令参数2
        for(; i < MAX_LINE && isspace(command[i] && command[i]); ++i); // 跳过前面的空白字符
        for(; i < MAX_LINE && !isspace(command[i] && command[i]); ++i)
            str2[j++] = command[i];
        str2[j] = '\0'; j = 0;
        if (strcmp(str2, "space")) //参数2错误
            return 0;
        char str3[MAX_LINE]; //命令参数3
        for(; i < MAX_LINE && isspace(command[i] && command[i]); ++i); // 跳过前面的空白字符
        for(; i < MAX_LINE && !isspace(command[i] && command[i]); ++i)
            str3[j] = command[i];
        str3[j] = '\0';
        if (!strcmp(str3, ""))
            return 0; //参数3不能为空字符串
        char buf[BUFFSIZE] = "space:create:";
        strcat(buf, str3);
        send_message(sockfd, buf, sizeof(buf));
        char response[BUFFSIZE];
        recv_message(sockfd, response, sizeof(response));
        if (!strncmp(response, "status:success", strlen("status:success"))){
            //status:success;data:space_id:...\0
            char space_id[MAX_LINE];
            int i = strlen("status:success;data:space_id:"), j = 0;
            for(; i < BUFFSIZE && response[i]; ++i)
                space_id[j++] = response[i];
            space_id[j] = '\0';
            printf("the space id you created is %s \n",space_id);
        }
        else {
            puts("create space fail. ");
        }        
        return 1;
    }

    else if (!strcmp(str1, "join")){
        char str2[MAX_LINE]; //命令参数2
        for(; i < MAX_LINE && isspace(command[i] && command[i]); ++i); // 跳过前面的空白字符
        for(; i < MAX_LINE && !isspace(command[i] && command[i]); ++i)
            str2[j++] = command[i];
        str2[j] = '\0';
        if (!strcmp(str2, ""))
            return 0; //参数2不能为空字符串
        char buf[BUFFSIZE] = "space:join:";
        strcat(buf, str2);
        send_message(sockfd, buf, sizeof(buf));
        char response[BUFFSIZE];
        recv_message(sockfd, response, sizeof(response));
        if (!strncmp(response, "status:success", strlen("status:success"))){
            printf("you are at space %s \n", str2);
            //进入房间
            join_space(str2);
        }
        else {
            puts("join space fail. ");
        }
    }
    else if (!strcmp(str1, "help")){
        puts("command : ");
        puts("  display user | display user all | display user online ");
        puts("  create space name ");
        puts("  join space_id ");
        puts("  help ");
    }
    else 
        return 0; //参数0错误
}

// 根据response打印用户
void display_user(char *response)
{
    //status:text;data:text
    char status[MAX_LINE];
    int i = 0;
    for (; (i < BUFFSIZE && response[i]) && (response[i] != ';'); ++i){
        int j = 0;
        status[j++] = response[i];
        status[j] = '\0';
    }
    if (strcmp(status, "status:success")){
        puts("you didn't login or response error");
        return;
    }
    //打印data部分
    //abc:1;aaa:0....\0 , 1为在线，0为不在线
    printf("%10s      %s \n", "name", "isOnline");
    i += strlen("data:"); // 跳过 data: 部分
    char name[MAX_NAME];
    int isOnline;
    int j = 0;
    for(; i < BUFFSIZE && response[i] ; ++i){
        if (response[i] != ':' && response[i] != ';')
            name[j++] = response[i];
        else if (response[i] == ':'){
            //name读完
            name[j] = '\0';
            j = 0;
            //读取在线状态
            isOnline = response[++i];
        }
        else {//读到';'
            printf("%10s      %s \n", name, isOnline ? "yes" : "no");
        }
    }
}

void join_space(char *space_id)
{
    strcpy(PS, "EChat ");
    strcat(PS, space_id);
    strcat(PS, ">");
    puts("input q to quit and > (if message is not q or >,not necessary)to send messages ");
    while (printf("%s", PS)){
        char ch;
        getchar(ch);
        char buf[BUFFSIZE] = "space:";
        if(ch == 'q'){
            strcat(buf, "leave:");
            strcat(buf, space_id);
            send_message(sockfd, buf, sizeof(buf));
            break;
        }
        else if (ch == '>'){
            strcat(buf, "send:");
            strcat(buf, space_id);
            strcat(buf, ":");
            gets( &buf[ strlen(buf) ] );
            send_message(sockfd, buf, sizeof(buf));
        }
        else {
            strcat(buf, "send:");
            strcat(buf, space_id);
            strcat(buf, ":");
            buf[ strlen(buf) ] = ch;
            gets( &buf[ strlen(buf) ] );
            send_message(sockfd, buf, sizeof(buf));
        }
    }
}
