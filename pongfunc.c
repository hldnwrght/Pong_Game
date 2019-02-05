//This holds the functions utilized in pongmain

#include <stdlib.h>
#include <termios.h>
#include <curses.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include "client_head.h"

//Initializes the updated paddle location
void init_paddle(int lpaddle, int rpaddle)
{
	move(lpaddle-3, 8);
	addch(219);
	move(lpaddle-2, 8);
	addch(219);
	move(lpaddle-1, 8);
	addch(219);
	move(lpaddle, 8);
	addch(219);
	move(lpaddle+1, 8);
	addch(219);
	move(lpaddle+2, 8);
	addch(219);
	move(lpaddle+3, 8);
	addch(219);
	
	move(rpaddle-3, 72);
	addch(219);
	move(rpaddle-2, 72);
	addch(219);
	move(rpaddle-1, 72);
	addch(219);
	move(rpaddle, 72);
	addch(219);
	move(rpaddle+1, 72);
	addch(219);
	move(rpaddle+2, 72);
	addch(219);
	move(rpaddle+3, 72);
	addch(219);
	move(LINES-1,COLS-1);
}

//Removes old paddle locations
void del_paddle(int lpaddle, int rpaddle)
{
	move(lpaddle-3, 8);
	addstr(" ");
	move(lpaddle-2, 8);
	addstr(" ");
	move(lpaddle-1, 8);
	addstr(" ");
	move(lpaddle, 8);
	addstr(" ");
	move(lpaddle+1, 8);
	addstr(" ");
	move(lpaddle+2, 8);
	addstr(" ");
	move(lpaddle+3, 8);
	addstr(" ");
	
	move(rpaddle-3, 72);
	addstr(" ");
	move(rpaddle-2, 72);
	addstr(" ");
	move(rpaddle-1, 72);
	addstr(" ");
	move(rpaddle, 72);
	addstr(" ");
	move(rpaddle+1, 72);
	addstr(" ");
	move(rpaddle+2, 72);
	addstr(" ");
	move(rpaddle+3, 72);
	addstr(" ");
}

//Termination function
void quit(int signum)
{
	term_Set(0);
	endwin();
	exit(0);
}

//Sets the terminal the the appropriate settings
void term_Set(int input)
{
	struct termios info;
	//Gets terminal attributes
	if ( tcgetattr(0,&info) == -1 )        
                perror("Error");

	if(input == 1)
	{
		//Turns off canonical mode
		info.c_lflag &= ~ICANON;
	}
	else
	{
		//Turns on canonical mode
		info.c_lflag |= ICANON;
	}


	//Sets new terminal settings
	if ( tcsetattr(0,TCSANOW,&info) == -1 ) 
               perror("Error");
}
