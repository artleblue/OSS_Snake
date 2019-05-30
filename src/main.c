#include <stdio.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "terminal.h"
#include "gamesystem.h"


int main( int argc, char **argv )
{
	Snake snake;
	Screen screen;
	GameCondition condition;
	Food food;

   struct winsize w;


   srand( time(NULL) );

	InitSnake( &snake );
	InitGameCondition( &condition );
    
	SetTerminal( &w );

	InitScreen( &screen, &w );
 
   tcsetattr( fileno(stdin), TCSANOW, &new_term_attr );
 
	while ( condition.quit != 2 )
   {
   	NewGame( &snake, &screen, &food );
    	condition.quit = 0;
     	Play( &snake, &screen, &condition, &food );
   }	

   Clear();

   tcsetattr( fileno(stdin), TCSANOW, &orig_term_attr );
    
	return 0;
}


