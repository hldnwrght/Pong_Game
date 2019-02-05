#This is the client makefile
all: client server

clean:
	rm -f client server

client: pongmain.c pongfunc.c client_head.h
	gcc -lcurses pongmain.c pongfunc.c -o client

server: servermain.c serverfunc.c server_head.h
	gcc -lcurses servermain.c serverfunc.c -o server

