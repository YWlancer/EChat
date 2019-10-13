#include "mysql.h"
#include <mysql/mysql.h>
#include <stdio.h>

MYSQL *conn; //该结构代表1个数据库连接的句柄

//其他翻译单元不可见
static const char * host = "localhost"; 
static const char * user = "root";
static const char * passwd = "12345678";
static const char * db = "test";
static unsigned int port = 3306;
static const char * unix_socket = NULL;
static unsigned long client_flag = 0;

int check_in_mysql(const char* sql)
{
    int ret = 0;
    /* 分配或初始化与mysql_real_connect()相适应的MYSQL对象。
    如果mysql是NULL指针，该函数将分配、初始化、并返回新对象。
    否则，将初始化对象，并返回对象的地址。*/
    conn = mysql_init(NULL);
    if(conn == NULL)
        exit(-1);
    /* 尝试与运行在主机上的MySQL数据库引擎建立连接 */
    conn = mysql_real_connect(conn, host, user, passwd, db, port, unix_socket, client_flag);
    if(conn == NULL)
        exit(-1);
    // 查询
    if(mysql_query(conn, sql) == 0){
        // 获取结果
        MYSQL_RES *result = mysql_store_result(conn);
        if (mysql_num_rows(result) != 0){
            ret = 1; // 结果集行数不为0
        }
        mysql_free_result(result); 
    }
    // 关闭连接
    mysql_close(conn);
    return ret;
}

int insert_user(const char* sql)
{
    
}
