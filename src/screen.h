#ifndef __SCREEN_H_
#define __SCREEN_H_

#include "location.h"

#include "snake.h"

#define MAP_MAX_WIDTH  300
#define MAP_MAX_HEIGHT 300

typedef struct Screen
{
	int width;
	int height;

}Screen;

char arena[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];

void InitScreen(Screen * screen, struct winsize * w)
{
	screen->width = -30;
	screen->height = -30;

	if(screen->width < 0 || screen->height < 0)
	{
		screen->height = w->ws_row - 1;
		screen->width = w->ws_col;
	}
	
	if(screen->width >= MAP_MAX_WIDTH)
	{	
		screen->width = MAP_MAX_WIDTH - 1;
	}
	if(screen->height >= MAP_MAX_HEIGHT)
	{
		screen->height = MAP_MAX_HEIGHT - 2;	// why -2, to disply scores.
	}
}

void Clear()
{
	printf("\033[H\033[J");
}

void MoveToPos(int x, int y)
{
	printf("\033[%d;%dH",(y+1),(x+1));
}



#endif
