#ifndef __SNAKE_H_
#define __SNAKE_H_

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

#endif
