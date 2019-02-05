//This holds the program for the server main
#include  <stdlib.h>
#include  <stdio.h>
#include  <unistd.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <sys/select.h>
#include  <netinet/in.h>
#include  <netdb.h>
#include  <time.h>
#include  <strings.h>
#include  <string.h>
#include  <signal.h>
#include  <curses.h>
#include  <errno.h>
#include "server_head.h"

int xpos, ypos, xdir, ydir, lpaddle, rpaddle, p1fd, p2fd;
int lpoints, rpoints, win1, win2;

int main(int ac, char *av[])
{
	struct  sockaddr_in   saddr;   
    struct  hostent         *hp;   
    char    hostname[256];     
    int     sock_id, sock_fd, messlen, activity; 
	int		p1fd, p2fd, maxfd;
    FILE    *sock_fp;               
    int     portnum, playercount = 0;
    char    message[256], state[15];
	fd_set  readfds;

    portnum = atoi(av[1]);

	//Ask kernel for socket
	sock_id = socket( PF_INET, SOCK_STREAM, 0 );    
    if ( sock_id == -1 )
		perror( "socket" );

	//Get host info and fill information
	bzero( (void *)&saddr, sizeof(saddr) );
	gethostname( hostname, 256 );      
    hp = gethostbyname( hostname );                                                  
    bcopy( (void *)hp->h_addr, (void *)&saddr.sin_addr, hp->h_length);
    saddr.sin_port = htons(portnum);       
    saddr.sin_family = AF_INET ;           

	//Bind
	if ( bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0 )
		perror( "bind" );

	//Allow incoming calls with queue size 
	if ( listen(sock_id, 5) != 0 )
		perror( "listen" );
	
	//Wait for players
	while(playercount<2)
	{
		if((sock_fd = accept(sock_id, NULL, NULL)) == -1)	//wait for call
			perror( "accept (1)" );

		if((messlen = read(sock_fd, message, 256)) < 0)
			perror("server read (1)");
		message[messlen] = '\0';

		if(strcmp(message, "join") == 0)
		{
			if(playercount == 0)
			{
				p1fd = sock_fd;	//Assign to player 1
				if(write(p1fd, "1", strlen("1")) != strlen("1"))
					perror("Write");
				playercount++;
			}
			else
			{
				//Collect client 2's fd and tell client 1 ready
				p2fd = sock_fd;
				if(write(p1fd, "ready", strlen("ready")) != strlen("ready"))
					perror("Write");
				if(write(p2fd, "ready", strlen("ready")) != strlen("ready"))
					perror("Write");
				playercount++;
			}
		}
	}

	//This initializes the variables used within the server
	signal(SIGALRM, update);
	xpos = 11; 
	ypos = 11; 
	xdir = 1; 
	ydir = 1; 
	lpaddle = 23; 
	rpaddle = 23;
	lpoints = 0; 
	rpoints = 0; 
	win1 = 0; 
	win2 = 0;

	if(set_ticker(80) == -1)
		perror("set ticker");
	
	//Loop for receiving activity from clients
	while(1)
	{
		//Select setup
		FD_ZERO(&readfds); //Clear socket set
		FD_SET(p1fd, &readfds); FD_SET(p2fd, &readfds);	//Add clients to read set
		if(p1fd > p2fd)
			maxfd = p1fd;
		else
			maxfd = p2fd;

		//Wait until a client sends a request
		activity = select(maxfd+1, &readfds, NULL, NULL, 0);
		if (activity < 0 && errno != EINTR)
			perror("select error");

		//If player 1 sent a request
		if(FD_ISSET(p1fd, &readfds))
		{
			if((messlen = read(p1fd, message, 255)) < 0)
				perror("server p1 read");
			message[messlen] = '\0';
		
			//If the message is a gamestate request, reply with the state
			if(strcmp(message, "state") == 0)
			{
				if(win1 == 1)	//First check if someone won
				{
					sprintf(state, "win1");
					if(write(p1fd, state, strlen(state)) != strlen(state))
						perror("Write");
				}
				else if(win2 == 1)
				{
					sprintf(state, "win2");
					if(write(p1fd, state, strlen(state)) != strlen(state))
						perror("Write");
				}
				else	//If nobody won, then send all game variables
				{
					sprintf(state,"%d,%d,%d,%d,%d,%d,%d,", xpos, ypos, lpaddle, rpaddle, lpoints, rpoints);
					if(write(p1fd, state, strlen(state)) != strlen(state))
					perror("Write");
				}
			}
			//If the message is a lpaddle up request
			else if(strcmp(message, "up") == 0)
			{
				if(lpaddle > 13)
					lpaddle--;
			}
		
			//If the message is a lpaddle down request
			else if(strcmp(message, "down") == 0)
			{
				if(lpaddle < 37)
				lpaddle++;
			}
		}

		//If player 2 sent a request
		if(FD_ISSET(p2fd, &readfds))
		{
			if((messlen = read(p2fd, message, 255)) < 0)
				perror("server p1 read");
			message[messlen] = '\0';

			//If the message is a gamestate request, reply with the state
			if(strcmp(message, "state") == 0)
			{
				if(win1 == 1)	//First check if someone won
				{
					sprintf(state, "win1");
					if(write(p2fd, state, strlen(state)) != strlen(state))
						perror("Write");
				}
				else if(win2 == 1)
				{
					sprintf(state, "win2");
					if(write(p2fd, state, strlen(state)) != strlen(state))
						perror("Write");
				}
				else	//If nobody won, then send all game variables
				{
					sprintf(state,"%d,%d,%d,%d,%d,%d,%d,", xpos, ypos, lpaddle, rpaddle, lpoints, rpoints);
					if(write(p2fd, state, strlen(state)) != strlen(state))
					perror("Write");
				}
			}
			//If the message is a lpaddle up request
			else if(strcmp(message, "up") == 0)
			{
				if(rpaddle > 13)
					rpaddle--;
			}
		
			//If the message is a lpaddle down request
			else if(strcmp(message, "down") == 0)
			{
				if(rpaddle < 37)
				rpaddle++;
			}
		}
		
	}
	
}
