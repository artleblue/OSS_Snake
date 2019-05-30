#include <stdio.h>
#include "snake.h"

/*뱀에 대한 기호 대입, 뱀의 길이와 속도 결정, 뱀의 방향 결정을 맡은 함수.*/
void InitSnake( Snake * snake )
{
	snake->SnakeChar_First = 'O';
	snake->SnakeChar = 'o';
	snake->SnakeChar_Tail = '.';
	
	for ( int i = 0 ; i < MAX_SIZE ; i++ )
	{
		snake->pos[i].x = -10;
		snake->pos[i].y = -10;
	}
	
	SetSnakeSize( snake, 3 );
	SetSnakeSpeed( snake, 80 );

	SetSnakeNextDirection( snake, RIGHT );
	SetSnakeDirection( snake, RIGHT );
}

/*뱀의 길이를 바꾸는 함수.*/
int SetSnakeSize( Snake * snake, int size_ )
{
	snake->size = size_;
}


int GetSnakeSize( Snake * snake )
{
	return snake->size;
}

/*뱀의 방향을 정하는 함수
 
 인자로 받은 방향으로 뱀의 방향을 설정한다.
 */
int SetSnakeDirection( Snake * snake, int direction_ )
{
	if ( direction_ != RIGHT
		&& direction_ != LEFT
		&& direction_ != UP
		&& direction_ != DOWN )
	{
	   return -1;
	}

	snake->direction = direction_;
	
	return 0;
}

int GetSnakeDirection( Snake * snake )
{
	return snake->direction;
}

/*뱀이 다음으로 움직일 방향을 정하는 함수
 
 인자로 받은 방향으로 뱀의 미래의 방향을 설정한다.
 */
int SetSnakeNextDirection( Snake * snake, int nextDirection_ )
{
	snake->new_direction = nextDirection_;
	
	return 0;
}

int GetSnakeNextDirection( Snake * snake )
{
	return snake->new_direction;
}

/*뱀의 속도를 정하는 함수
 
 인자로 받은 속도로 뱀의 속도를 설정한다.
 */
int SetSnakeSpeed( Snake * snake, int speed_ )
{
	if ( speed_ < 0 )
	{
		return -1;
	}

	snake->speed = speed_;
	
	return 0;
}

int GetSnakeSpeed( Snake * snake )
{
	return snake->speed;
}

/*뱀이 바라보는 방향으로 뱀을 움직이는 함수.
 
 뱀이 오류없이 움직인다면 true를 반환하고,
 뱀이 움직일 수 없다면 false를 반환한다.
 */
int MoveSnakePos( Snake * snake )
{
	const int SNAKE_SIZE = GetSnakeSize( snake );
	const int SNAKE_DIRECTION = GetSnakeDirection( snake );

	int add_x = 0;
	int add_y = 0;
	

	switch ( SNAKE_DIRECTION )
	{
		case RIGHT:
		   add_x = 1;
		   break;
		
		case UP:
		   add_y = 1;
		   break;
		
		case DOWN:
		   add_y = -1;
		   break;
		
		case LEFT:
		   add_x = -1;
		   break;
	}

	Pos head = snake->pos[0];
	Pos tail = snake->pos[SNAKE_SIZE];

	MoveToPos( tail.x, tail.y );
	printf( " " );

	MoveToPos( head.x, head.y );
	printf( "%c", snake->SnakeChar );

	for ( int i = SNAKE_SIZE ; i > 0 ; i-- )
	{
		snake->pos[i].x = snake->pos[i-1].x;
		snake->pos[i].y = snake->pos[i-1].y;
	}

	head.x += add_x;
	head.y += add_y;

	snake->pos[0].x += add_x;
	snake->pos[0].y += add_y;

	MoveToPos( snake->pos[0].x, snake->pos[0].y );
	printf("%c", snake->SnakeChar_First);
	
	MoveToPos( snake->pos[SNAKE_SIZE].x, snake->pos[SNAKE_SIZE].y );
	printf( "%c", snake->SnakeChar_Tail );
	
	return 0;
}


/*머리를 포함한 뱀이 주어진 위치와 충돌하였는지 검사하는 함수.*/
int CheckSnakeCollision( Pos px, Snake * snake, Screen * screen )
{
	const int SNAKE_SIZE = GetSnakeSize( snake );

	if ( arena[px.x][px.y] != ' ' )
	{
		return 1;
	}
	if ( px.x < 1 )
	{ 
		return 1;
	}
	if ( px.y < 1 )
	{
		return 1;
	}
	if ( px.x > screen->width - 2 )
	{
		return 1;
	}
	if ( px.y > screen->height - 1 )
	{	
		return 1;
	}

	for ( int i = 0 ; i < SNAKE_SIZE ; i++ )
	{
		if ( snake->pos[i].x == px.x
			  && snake->pos[i].y == px.y )
		{
			return 1;
		}
	}

	return 0;
}


/*뱀의 몸통이 주어진 위치와 충돌하였는지 검사하는 함수*/
int CheckSnakeCollisionWithoutHead( Pos px, Snake * snake, Screen * screen )
{
	const int SNAKE_SIZE = GetSnakeSize( snake );

	if ( arena[px.x][px.y] != ' ' )
	{
		return 1;
	}
	if ( px.x < 1 )
	{  
		return 1;
	}
	if ( px.y < 1 )
	{ 
		return 1;
	}
	if ( px.x > screen->width - 2 )
	{
		return 1;
	}
	if ( px.y > screen->height - 1 )
	{
		return 1;
	}

	for ( int i = 1 ; i < SNAKE_SIZE ; i++ )
	{
		
		if ( snake->pos[i].x == px.x
			  && snake->pos[i].y == px.y )
		{
			return 1;
		}
	}

	return 0;
}


void SetSnakeInitPos( Snake * snake, const int SCREEN_WIDTH, const int SCREEN_HEIGHT )
{
	const int SNAKE_SIZE = GetSnakeSize( snake );

	int start_x = SCREEN_WIDTH / 2;
	int start_y = SCREEN_HEIGHT / 2;
	

	for ( int i = 0 ; i < SNAKE_SIZE ; i++)
	{
		if ( start_x < 0 ) // 스크린이 너무 작을 경우
		{
			Fatal( "The screen is too small." );
		}

		snake->pos[i].x = start_x--;
		snake->pos[i].y = start_y;
	}
}

