#include <stdio.h>
#include "draw.h"


/*뱀의 길이를 구해 해당 크기만큼 뱀을 나타내는 기호를 출력하는 함수

 뱀의 정보 사용을 위해 Snake형 snake를 인자로 받는다.*/
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
      
		//뱀을 나타내는 부분 중 다음 칸으로 이동한다.
		MoveToPos( snake->pos[remaining].x, snake->pos[remaining].y );
      printf("%c", symbol);
   }

   fflush( stdout );
}

/*뱀의 길이를 점수로 변환 후 점수, 옵션, 메뉴를 출력하는 함수.*/
void DrawGameInfo( Snake * snake, Screen * screen )
{
	const int SCREEN_HEIGHT = 31;
	const int SNAKE_SIZE = GetSnakeSize(snake);
	const int SCORES = SNAKE_SIZE - 3;


	MoveToPos( 0, SCREEN_HEIGHT ); 

	printf( "Score: %i | t - turbo | x - Quit the game | p - pause the game ", SCORES );
}

/*screen의 너비와 높이만큼 맵을 출력하는 함수
 
 너비와 높이의 사용을 위해 Screen형 screen을 인자로 받는다.*/
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
