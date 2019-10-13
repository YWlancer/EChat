# all:mysql

# mysql:mysql.c
# 	gcc mysql.c -o mysql -lmysqlclient -L/usr/lib64/mysql

all:server EChat

EChat:EChat.c
	gcc EChat.c socket.c -o EChat

server:server.c
	gcc server.c -o server

clean:
	rm server EChat

