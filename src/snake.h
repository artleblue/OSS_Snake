/*
 * 뱀의 정보를 가지고 있으며
 * 뱀의 길이, 속도, 방향을 설정하거나 반환,
 * 뱀의 충돌을 검사하는 함수를 포함하는 헤더.
 *
 * 스크린의 정보인 screen.h와 위치정보인 location.h를 포함하고 있다.
 */

#ifndef __SNAKE_H_
#define __SNAKE_H_

#include "location.h"
#include "screen.h"

#define MAX_SIZE   800


/*뱀의 정보*/
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
void SetSnakeInitPos( Snake * snake, const int SCREEN_WIDTH, const int SCREEN_HEIGHT );

#endif
