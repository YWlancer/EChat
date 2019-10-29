all:EChat

EChat:EChat.c socket.o
	gcc EChat.c socket.o -o EChat

socket.o:socket.c
	gcc socket.c -o socket.o

clean:
	rm socket.o EChat
