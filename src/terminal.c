#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "terminal.h"

void SetTerminal( struct winsize * w )
{
	tcgetattr( fileno( stdin ), &orig_term_attr );

   memcpy( &new_term_attr, &orig_term_attr, sizeof( struct termios ) );

  	new_term_attr.c_lflag &= ~(ECHO|ICANON);
  	new_term_attr.c_cc[VTIME] = 0;
 	new_term_attr.c_cc[VMIN] = 0;

  	ioctl( STDOUT_FILENO, TIOCGWINSZ, w );
}

