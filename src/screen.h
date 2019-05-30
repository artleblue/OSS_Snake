#ifndef __SCREEN_H_
#define __SCREEN_H_

#define MAP_MAX_WIDTH  300
#define MAP_MAX_HEIGHT 300


typedef struct Screen
{
	int width;
	int height;

}Screen;

char arena[ MAP_MAX_HEIGHT ][ MAP_MAX_WIDTH ];

void InitScreen( Screen * screen, struct winsize * w );
void Clear();
void MoveToPos( int x, int y );

#endif
