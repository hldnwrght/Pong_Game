//This holds the functions used in the server
#include "server_head.h"
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>

#define playto 11

extern int xpos, ypos, xdir, ydir, lpaddle, rpaddle, p1fd, p2fd;
extern int lpoints, rpoints, win1, win2;

//Updates the gamestate
void update(int signum)
{
	//First check to see if someone has won already
	if(win1 == 1 || win2 == 1)	//If so, no need to check for collisions
		return;
	
	//Check for collision and update ball directions accordingly
	if((xpos >= 75) || (xpos <= 5))
		xdir *= -1;
	if((ypos >= 40) || (ypos <= 10))
		ydir *= -1;
	if((xpos == 9)&&((ypos == lpaddle)||(ypos == lpaddle-2)||(ypos == lpaddle-1)||(ypos == lpaddle+1)||(ypos == lpaddle+2)
		||(ypos == lpaddle+3)||(ypos == lpaddle-3)))
		xdir *= -1;
	if((xpos == 71)&&((ypos == rpaddle)||(ypos == rpaddle-2)||(ypos == rpaddle-1)||(ypos == rpaddle+1)||(ypos == rpaddle+2)
		||(ypos == rpaddle+3)||(ypos == rpaddle-3)))
		xdir *= -1;
		
	if(xpos == 75)	//If player 1 scored
	{
		lpoints++;
		xpos = 45;
		ypos = 25;
	}
	
	if(xpos == 5)	//If player 2 scored
	{
		rpoints++;
		xpos = 45;
		ypos = 25;	
	}
	
	if(lpoints >= playto)	//Check to see if someone has won now
		win1 = 1;
	if(rpoints >= playto)
		win2 = 1;

	
	//Update locations
	xpos += xdir;
	ypos += ydir;
}

//Ticker function for time interval 
int set_ticker( int n_msecs )
{
        struct itimerval new_timeset;
        long    n_sec, n_usecs;

        n_sec = n_msecs / 1000 ;                /* int part     */
        n_usecs = ( n_msecs % 1000 ) * 1000L ;  /* remainder    */

        new_timeset.it_interval.tv_sec  = n_sec;        /* set reload       */
        new_timeset.it_interval.tv_usec = n_usecs;      /* new ticker value */
        new_timeset.it_value.tv_sec     = n_sec  ;      /* store this       */
        new_timeset.it_value.tv_usec    = n_usecs ;     /* and this         */

        return setitimer(ITIMER_REAL, &new_timeset, NULL);
}
