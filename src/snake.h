#ifndef __SNAKE_H_
#define __SNAKE_H_

#include "location.h"
#include "screen.h"

#define MAX_SIZE   800


typedef struct Snake
{
	char SnakeChar_First;
	char SnakeChar;
	char SnakeChar_Tail;
	
	int size;
	int speed;
	int direction;
	int new_direction;
	
	Pos pos[ MAX_SIZE ];

}Snake;


void InitSnake( Snake * snake );
int SetSnakeSize( Snake * snake, int size_ );
int GetSnakeSize( Snake * snake );
int SetSnakeDirection( Snake * snake, int direction_ );
int GetSnakeDirection( Snake * snake );
int SetSnakeNextDirection( Snake * snake, int nextDirection_ );
int GetSnakeNextDirection( Snake * snake );
int SetSnakeSpeed( Snake * snake, int speed_ );
int GetSnakeSpeed( Snake * snake );
int MoveSnakePos( Snake * snake );
int CheckSnakeCollision( Pos px, Snake * snake, Screen * screen );
int CheckSnakeCollisionWithoutHead( Pos px, Snake * snake, Screen * screen );
int SetSnakeInitPos( Snake * snake, const int SCREEN_WIDTH, const int SCREEN_HEIGHT );

#endif
