//This is the main of the client side of pong
//This gets the connection of the server, sends and receives data from the server
//Then it updates the screen
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netdb.h>
#include <string.h>
#include <curses.h>
#include <signal.h>
#include <errno.h>
#include "client_head.h"

int xpos, ypos, lpaddle, rpaddle, sock_id, lpoints, rpoints;
char player;

int main(int ac, char *av[])
{
	struct sockaddr_in  servadd;       
    struct hostent      *hp;            
    int    sock_fd, activity, maxfd;
	FILE   *sock_fp;
    char   message[256], score1[10], score2[10], n;             
    int    messlen, x;
	char   *tok;
	fd_set readfds, writefds;

	//initialize variables
	xpos = 11; 
	ypos = 11;
	lpaddle = 23; 
	rpaddle = 23;

	//Get a socket
	if((sock_id = socket( AF_INET, SOCK_STREAM, 0 )) == -1) 
		perror( "socket" );
		
	//Setup server connection
	bzero( &servadd, sizeof( servadd ) );
	if((hp = gethostbyname( av[1] )) == NULL)	//get host ip
		perror(av[1]);
	bcopy(hp->h_addr, (struct sockaddr *)&servadd.sin_addr, hp->h_length);
	servadd.sin_port = htons(atoi(av[2]));	//Port num
	servadd.sin_family = AF_INET ;			//Socket type

	//Request to be added to the game
	if( connect(sock_id,(struct sockaddr *)&servadd, sizeof(servadd)) !=0)
		perror( "connect" );
	if(write(sock_id, "join", strlen("join")) != strlen("join"))
		perror("Join Write");
	if((messlen = read(sock_id, message, BUFSIZ)) < 0)
		perror("Client join read");
	message[messlen] = '\0';
	
	//Waiting if player 1
	if(strcmp(message, "1") == 0)
	{
		printf("\n\nWaiting for an opponent...\n");
		if((messlen = read(sock_id, message, BUFSIZ)) < 0)
			perror("Client wait read");
		message[messlen] = '\0';
		printf("\n%s, Player 1!\n", message);
	}
	

	//Setup screen for game
	initscr();
	noecho();
	term_Set(1);
	//sets up a quit signal to end game
	signal(SIGINT, quit);

	//Initializes the board
	for(x=4;x<=76;x++)	//Top and bottom of board
	{
		move(9, x);
		addstr("=");
		move(41, x);
		addstr("=");
	}
	for(x=9;x<=41;x++)	//Left and right of board
	{
		move(x, 4);
		addstr("|");
		move(x, 76);
		addstr("|");
	}

	//This is the section that 
	while(1)
	{	
		//Setup for select
		FD_ZERO(&readfds); FD_ZERO(&writefds); //Clear socket sets
		FD_SET(sock_id, &writefds);	//Add server to write set
		FD_SET(0, &readfds);		//Add stdin to read set
		if(sock_id > 0)
			maxfd = sock_id;
		else
			maxfd = 0;


		//See which fds are ready 
		activity = select(maxfd+1, &readfds, &writefds, NULL, 0);
		if (activity < 0 && errno != EINTR)
			perror("select error");

		
		//If user entered something:
		if(FD_ISSET(0, &readfds))
		{
			if((messlen = read(0, message, 255)) < 0)
				perror("stdin read");
			n = message[0];

			//Move lpaddle up
			if(n == 'w')
			{
				//Tell the server
				if(write(sock_id, "up", strlen("up")) != strlen("up"))
					perror("Client lpaddle up write");
			}
			//Move lpaddle down
			else if(n == 's')
			{
				//Tell the server
				if(write(sock_id, "down", strlen("down")) != strlen("down"))
					perror("Client lpaddle down write");
			}
		}

		
		//Ask for current state of the game
		if(write(sock_id, "state", strlen("state")) != strlen("state"))
			perror("Client state request");

		//Read in gamestate
		if((messlen = read(sock_id, message, BUFSIZ)) < 0)
			perror("Client state read");
		message[messlen] = '\0';

		//First check to see if someone won the game, and display if they did
		//If player 1 wins go through player 1 win sequence and exit game
		if(strcmp(message, "win1") == 0)
		{
			move(8, 32);
			addstr("Press Q to end the game");
			move(24, 10);
			addstr("Left Paddle Is The Winner!!!!");
			lpoints++;
			sprintf(score1, "Score: %d", lpoints);
			move(8, 8);
			addstr(score1);
			move(LINES-1,COLS-1);
			refresh();
			while(n = getch())
			{
				if(n == 'q')
				{
					term_Set(0);
					endwin();
					exit(0);
				}
			}
		}
		//If player 2 wins go through player 2 win sequence and exit game
		else if(strcmp(message, "win2") == 0)
		{
			move(8, 32);
			addstr("Press Q to end the game");
			move(24, 60);
			addstr("Right Paddle Is The Winner!!!!");
			rpoints++;
			sprintf(score2, "Score: %d", rpoints);
			move(8, 64);
			addstr(score2);
			move(LINES-1,COLS-1);
			refresh();
			while(n = getch())
			{
				if(n == 'q')
				{
					term_Set(0);
					endwin();
					exit(0);
				}
			}
		}
		else	//If nobody won the game, referesh the screen with received updates
		{
			//Update positions according to received data
			tok = strtok(message, ",");
			xpos = atoi(tok);
			tok = strtok(NULL, ",");
			ypos = atoi(tok);
			tok = strtok(NULL, ",");
			lpaddle = atoi(tok);
			tok = strtok(NULL, ",");
			rpaddle = atoi(tok);
			tok = strtok(NULL, ",");
			lpoints = atoi(tok);
			tok = strtok(NULL, ",");
			rpoints = atoi(tok);

			//Updates the left paddles points
			sprintf(score1, "Score: %d", lpoints);
			move(8, 8);
			addstr(score1);
			//Updates the right paddles points
			sprintf(score2, "Score: %d", rpoints);
			move(8, 64);
			addstr(score2);
			//Updates the y and x location for the ball
			move(ypos, xpos);
			addstr("0");


			//reinitializes the left and right paddle 
			init_paddle(lpaddle, rpaddle);
			refresh();

			//Erase old locations for paddle and the ball
			move(ypos, xpos);	
			addstr(" ");
			del_paddle(lpaddle, rpaddle);
		}	
	}
}
