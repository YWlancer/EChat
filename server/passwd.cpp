#include "passwd.h"
#include <unordered_map>
#include <string>
#include "mysql.h"
using namespace std;

static unordered_map<string, string> passwords; 

/****
 利用C API连接数据库查询，应分三种情况
 1.用户名在哈希表中，根据查询结果直接返回
 2.不在哈希表中，且查询数据库正确，取出（用户名和）密码，放入哈希表
 3.不在哈希表中且数据库中无此用户名，直接返回
 补充：哈希表容量应有限制，当超过容量应采取一定策略排除已存用户名
*/
int check_passwd(char *cstr_name, char *cstr_passwd)
{
    string name(cstr_name), passwd(cstr_passwd);
    // 先在unordered_map中查询，减少查询数据库开销
    auto search = passwords.find(name);
    if (search != passwords.end()) {
        string real_passwd = search->second;
        if (passwd == real_passwd){
            return 1; // 在哈希表中且密码正确
        } else {
            return 0; // 在哈希表中且密码不正确
        }
    } else {
        // not found, 查询数据库
        string sql = "select * from user where name = ";
        sql += name;
        sql += " and passwd = ";
        sql += passwd;
        if (check_in_mysql(sql.c_str()) {
            // 用户名密码在数据库中
            passwords,insert(make_pair(name, passwd));
            return 1;
        }
        else {
            // 不在数据库中
            return 0;
        }
    }
}

int add_user(char *cstr_name, char *cstr_passwd)
{
    // 判断用户名是否已被注册
    // 先在哈希表中查询
    string name(cstr_name), passwd(cstr_passwd);
    auto search = passwords.find(name);
    if (search != passwords.end()) {
        // 在哈希表中，用户已存在不允许注册
        return 0;
    }
    // 查询数据库
    string sql = "select * from user where name = ";
    sql += name;
    if (check_in_mysql(sql.c_str()) {
        // 用户名在数据库中，不允许注册该用户名
        // 应该取出用户名密码插入哈希表
        return 0;
    }
    else {
        name = ('\"' + name + '\"');
        passwd = ('\"' + passwd + '\"');
        string insert_sql = "insert into user values( " + name + ',' + passwd + ')';
        return insert_user(insert_sql);
    }
}

