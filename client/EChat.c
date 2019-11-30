/* 标准库头文件 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* 操作系统头文件 */
#include <unistd.h>

/* 其他头文件 */
#include "terminal.h"

#define USER_NAME_MAX   20
#define USER_PASS_MAX   32
#define Streq(s1, s2) (strcmp((s1), (s2)) == 0)

#define Fgets(str, n, file) do {        \
    fgets(str, n, file);                \
    char *find = strchr(str, '\n');     \
    if(find)                            \
        *find = '\0';                   \
} while(0)

// 用全局变量存储用户名和密码，登录成功后需清除密码
static char user_name[USER_NAME_MAX + 1]; // +1 for null character
static char user_pass[USER_PASS_MAX + 1]; // +1 for null character

void check_args(int argc, char *argv[]);
int read_username();
int read_passwd();

int main(int argc, char *argv[])
{
    check_args(argc, argv);
    if (read_username() != 0 || read_passwd() != 0 ) {
        exit(1);
    }

    exit(0);
}

void check_args(int argc, char *argv[])
{
    if (argc == 1) {
        return;
    }
    if (argc == 2 && Streq(argv[1], "-h")) {
        puts("usage : EChat [ options ] ");
        puts("options : ");
        puts("  -h               display this information ");
        puts("  -u name          use the name to log in ");
        puts("  -r               sign up for EChat ");
        exit(0);
    } else if (argc == 3 && Streq(argv[1], "-u")) {
        if (strlen(argv[2]) > USER_NAME_MAX) {
            printf("error : the max characters of user's name is %d\n", USER_NAME_MAX);
            exit(1);
        }
        strncpy(user_name, argv[2], USER_NAME_MAX + 1);
    } else if (argc == 2 && Streq(argv[1], "-r")) {
        // todo
    } else {
        puts("error, input -h to view usage ");
        exit(1);
    }
}

int read_username()
{
    if (strlen(user_name) == 0) {
        printf("login: ");
        char line[USER_NAME_MAX + 2] = { 0 }; // +2 用来检测输入字符数
        Fgets(line, USER_NAME_MAX + 2, stdin);
        if (line[USER_NAME_MAX] != '\0') { // 若输入超过最大字符数，则下一个个字符不为\0
            printf("error : the max characters of user's name is %d\n", USER_NAME_MAX);
            return -1;
        }
        int i;
        for (i = 0; i < USER_NAME_MAX; i++) {
            if (isspace(line[i])) {
                puts("Username does not contain space characters");
                return -1;
            }
        }
        strncpy(user_name, line, USER_NAME_MAX + 1);
    }
    return 0;
}

int read_passwd()
{
    printf("passwd: ");
    char line[USER_PASS_MAX + 2] = { 0 }; // +2 用来检测输入字符数

    if (set_echo(STDIN_FILENO, SET_ECHO_OFF) != 0) { // 关闭回显
        return -1;
    } 
    Fgets(line, USER_PASS_MAX + 2, stdin);
    if (set_echo(STDIN_FILENO, SET_ECHO_ON) != 0) { // 开启回显
        return -1;
    }

    if (line[USER_PASS_MAX] != '\0') { // 若输入超过最大字符数，则下一个个字符不为\0
        printf("error : the max characters of user's pass is %d\n", USER_PASS_MAX);
        return -1;
    }
    strncpy(user_pass, line, USER_PASS_MAX + 1);
    return 0;
}
