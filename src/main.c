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
	/*변수들은 뱀, 스크린, 게임상태, 음식의 정보를 가지고 있다.*/
	Snake snake;
	Screen screen;
	GameCondition condition;
	Food food;
	
	/*윈도우 사이즈*/
   struct winsize w;

	
   srand( time(NULL) );
	
	/*초기설정.*/
	InitSnake( &snake );
	InitGameCondition( &condition );
   
	/*터미널 설정*/
	SetTerminal( &w );

	InitScreen( &screen, &w );
 	
	/*터미널 파일의 속성을 &new_term_attr에 저장한다.*/
   tcsetattr( fileno(stdin), TCSANOW, &new_term_attr );
 
	/*게임이 종료상태가 아니면 게임을 플레이한다.*/
	while ( condition.quit != 2 )
   {
   	NewGame( &snake, &screen, &food );
    	condition.quit = 0;
     	Play( &snake, &screen, &condition, &food );
   }	

   Clear();
	
	/*터미널 파일의 속성을 &orig_term_attr에 저장한다.*/
   tcsetattr( fileno(stdin), TCSANOW, &orig_term_attr );
    
	return 0;
}


