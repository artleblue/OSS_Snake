#include <stdio.h>
#include <sys/ioctl.h>
#include "screen.h"

/*윈도우 크기에 따라 스크린을 출력하는 함수.*/
void InitScreen( Screen * screen, struct winsize * w )
{
	screen->width = -30;
	screen->height = -30;

	if( screen->width < 0 || screen->height < 0 )
	{
		screen->height = w->ws_row - 1;
		screen->width = w->ws_col;
	}
	
	if( screen->width >= MAP_MAX_WIDTH )
	{	
		screen->width = MAP_MAX_WIDTH - 1;
	}

	if( screen->height >= MAP_MAX_HEIGHT )
	{
		screen->height = MAP_MAX_HEIGHT - 2;
	}
}

/*커서를 화면 시작부분에 위치시키고 화면을 지우는 함수.*/
void Clear()
{
	printf( "\033[H\033[J" ); // ANSI 코드
}

/*지정 위치에 글자를 출력하는 함수.*/
void MoveToPos( int x, int y )
{
	printf( "\033[%d;%dH", ( y + 1 ), ( x + 1 ) ); // ANSI 코드
}
