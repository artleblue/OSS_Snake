#include <stdio.h>
#include "snake.h"

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

int SetSnakeSize( Snake * snake, int size_ )
{
	snake->size = size_;
}

int GetSnakeSize( Snake * snake )
{
	return snake->size;
}

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

int SetSnakeNextDirection( Snake * snake, int nextDirection_ )
{
	snake->new_direction = nextDirection_;
	
	return 0;
}

int GetSnakeNextDirection( Snake * snake )
{
	return snake->new_direction;
}

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

int MoveSnakePos( Snake * snake )
{
   // Moves snake into a pos that snake is looking at
   // return: when snake moves without erros, returns true.
   // return: when snake can't move, return false.
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

	// why it is not snakesize -1
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

int SetSnakeInitPos( Snake * snake, const int SCREEN_WIDTH, const int SCREEN_HEIGHT )
{
	const int SNAKE_SIZE = GetSnakeSize( snake );
	int start_x = SCREEN_WIDTH / 2;
	int start_y = SCREEN_HEIGHT / 2;
	
	for ( int i = 0 ; i < SNAKE_SIZE ; i++)
	{
		if ( start_x < 0 )
		{
			Fatal( "The screen is too small." );
		}

		snake->pos[i].x = start_x--;
		snake->pos[i].y = start_y;
	}
}

