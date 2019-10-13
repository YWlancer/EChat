# 概要设计0.0.1 2019/10/13 

EasyChat  

client.c 功能描述  
1.1 echat 建立与服务器的连接 登录  
1.2 echat -h 说明用法  
1.3 echat register 注册（暂不实现）  
    登录成功 提示符echat >>   
2.1 exit 退出  
2.2 search 查找已有聊天室  
2.3 join id 加入已有聊天室  
    进入聊天室后提示符 echat room >>   
3.1 send  
3.2 exit  

server.c 功能描述  
监听端口，处理消息  

程序语言 C/C++  
开发环境 Linux  

## 报文规则  
客户端发送  
login:name:passwd 登录报文，name与passwd均不含：，报文均以\0结尾  
服务器端接受报文，对于未登录的客户端，仅处理login报文  
对于登录成功的客户端，忽略登录报文  
服务器用集合（或其它数据结构）储存已登录客户  
服务器返回报文：login:success or login:fail  
  
如前所述，对于成功登录的客户端，可以发送其它命令报文（未登录成功也可以发送，但服务器不处理）  
display:user:all 列出所有用户，其状态为online or disable(不可见，离线或隐身，隐身暂时不考虑)  
display:user:online 列出在线用户  
display:space 列出不加密的聊天室（后续功能会对聊天室添加密码功能）  
space:create:name 创建聊天室 name不唯一，有唯一的id  
space:join:id 加入聊天室  
space:leave:id （不加id，则服务器判断处于哪个id，这取决于我们是否支持同时加入多聊天室）  
    聊天室所有用户都离开则关闭聊天室，聊天室id短暂保留，leave是主动离开  
    聊天室不区分权限，（没有群主）  
space:send : id :message 发送消息 （同上，若不支持多聊天室，id不必加）  
  
总结：服务器目前支持三类报文  
login 登录 （TCP断开连接则认为是注销）  
display 查询 查询用户或聊天室，服务器只处理已登录客户  
space 聊天室相关  
  
服务器端报文  
login:success | fail  
对于没登录的客户端login报文,成功或失败，发送此报文  
对于已登录的客户端login报文，不发送  
对于未登录客户端的其他报文（非login）发送login:fail  
  
对于已登录的客户端的其他报文  
发送  
status:text;data: text  
text是json格式字符串，以{开头 }结尾  
status和data中间以;隔开  
status描述用户权限，错误信息等，data是客户端主要展示的内容  

上诉报文字段，格式，分隔符仅供参考  

## 客户端程序流程  
EChat [-r -h -u [name] ] -r注册 -h帮助 -u以用户名name登录  
login:  
passwd:  

成功后提示符 EChat > 接收命令  
q 退出，断开TCP,服务器标记此用户（sockfd而不是用户名)未登录  
display user all  
display user online  
display space  
与上诉报文对应，发送display： 报文  
create space name  
发送创建聊天室请求，服务器分配id  
join id  
发送加入某个聊天室请求  
help  
客户端打印用法，display create join等  
其他命令，告知用户命令不合法，输入help可查看命令  
  
成功进入聊天室后提示符 EChat spacename   
键入单独q,发送退出聊天室报文，退出聊天室  
键入>, >后面均为要发送的内容，无需转义  
（或者提示符是>> 默认是要发送消息，删除一个>可以键入q，推荐上面那种做法）  
  
## 服务器端程序  
数据结构与函数  
一个set or map 存储已客户  
一个map存储部分用户名密码，起到缓存作用，不用频繁读数据库  
一个struct维护space  
一个函数结束、分拣报文  
几个函数处理报文，组装response  
一个函数回复报文  

## notice  
数据结构可以用C++标准库，C外部库或自己实现（倾向于前两种）  


## 数据库
database test;
table user;
name(key) passwd
