#ifndef __DRAW_H_
#define __DRAW_H_

#include "snake.h"


void DrawSnake( Snake * snake )
{
	const int SNAKE_SIZE = GetSnakeSize(snake); 
   int remaining = -1;


	while ( remaining++ < SNAKE_SIZE )
   {
   	char symbol = snake->SnakeChar;


   	if ( remaining == 0 )
		{
			symbol = snake->SnakeChar_First;
		}
      else if ( remaining == SNAKE_SIZE )
		{	
			symbol = snake->SnakeChar_Tail;
		}
		
		if ( snake->pos[remaining].x < 0 
			|| snake->pos[remaining].y < 0 )
		{
			break;
		}
        
		MoveToPos( snake->pos[remaining].x, snake->pos[remaining].y );
      printf("%c", symbol);
   }

   fflush( stdout );
}

void DrawGameInfo( Snake * snake, Screen * screen )
{
	const int SCREEN_HEIGHT = 31;
	const int SNAKE_SIZE = GetSnakeSize(snake);
	const int SCORES = SNAKE_SIZE - 3;


	MoveToPos(0, SCREEN_HEIGHT ); 

	printf("Score: %i | t - turbo | x - Quit the game | p - pause the game ", SCORES);
}

void DrawMap( Screen * screen )
{
	int x,y;
   x = 0;

   while ( x - 1 < screen->width )
   {
		y = 0;

      do
		{
			if ( x == 0 || y == 0 
					|| x >= screen->width 
					|| y >= screen->height-1 )
         {
				arena[x][y] = '*';
            MoveToPos(x,y);
            printf("*");
         }

      } while ( y++ < screen->height - 1 );

      x++;
   }
}

#endif
