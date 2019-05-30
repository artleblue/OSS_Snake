#include <stdio.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "location.h"
#include "snake.h"
#include "screen.h"
#include "food.h"
#include "draw.h"



typedef struct GameCondition
{
	int turbo;
	int paused;
	int quit;

}GameCondition;


// Game System
Pos GetRandomPos(int width, int height);
int LocateFood( Snake * snake, Food * food, Screen * screen );
int rand_lim(int limit);
void play(Snake * snake, Screen * screen,GameCondition * condition, Food * food);
int fetch_key();
void fatal(char *reason);
void game_over(Screen * screen, GameCondition * condition);
void new_game( Snake * snake, Screen * screen, Food * food );
void InitGameCondition(GameCondition * condition);

//Terminal
void SetTerminal(struct winsize * w);


struct termios orig_term_attr;
struct termios new_term_attr;




int main(int argc, char **argv)
{
	Snake snake;
	Screen screen;
	GameCondition condition;
	Food food;

    	struct winsize w;

    	srand(time(NULL));

	InitSnake(&snake);
	InitGameCondition(&condition);
    
	SetTerminal(&w);

	InitScreen(&screen, &w);
 
   	 tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
 
	while (condition.quit != 2)
    	{
       	 	new_game( &snake, &screen, &food );
        	condition.quit = 0;
        	play( &snake, &screen, &condition, &food );
    	}	

    	Clear();
    	tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
    
	return 0;
}




void SetTerminal(struct winsize * w)
{
	tcgetattr(fileno(stdin), &orig_term_attr);
    	memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    	new_term_attr.c_lflag &= ~(ECHO|ICANON);
    	new_term_attr.c_cc[VTIME] = 0;
   	new_term_attr.c_cc[VMIN] = 0;
    	ioctl(STDOUT_FILENO, TIOCGWINSZ, w);
}


void InitGameCondition(GameCondition * condition)
{
	condition->paused = 0;
	condition->quit = 0;
	condition->turbo = 0;
}

void new_game( Snake * snake, Screen * screen, Food * food )
{
    int ax = 0;
    int ay = 0;
    
    	SetSnakeSize( snake, 3 );
	InitFood( food );

	do
	{
        ay = 0;
        do
        {
            arena[ax][ay] = ' ';
        } while (++ay < MAP_MAX_WIDTH);
    } while (++ax < MAP_MAX_HEIGHT);
  

	for ( int i = 0; i < MAX_SIZE; i++ )
	{
		snake->pos[i].x = -10;
		snake->pos[i].y = -10;
	}

	SetSnakeInitPos(snake, screen->width, screen->height); 

    	Clear();
    	draw_map(screen);
    	LocateFood(snake, food, screen);
    	DrawGameInfo(snake, screen);
	draw_snake(snake);
}

void game_over(Screen * screen, GameCondition * condition)
{
    MoveToPos(screen->width / 2, screen->height / 2);
    printf("* GAME OVER *");
    MoveToPos((screen->width / 2) - 10, (screen->height / 2) + 2);
    printf("Press x to exit or n for new game");
    int key = -1;
    while (key != 110 && key != 120)
    {
        key = fetch_key();
        usleep(200);
    }
    if (key == 120)
        condition->quit = 2;
    else
        condition->quit = 1;
}

int fetch_key()
{
    return fgetc(stdin);
}

void fatal(char *reason)
{
    printf("FATAL: %s\n", reason);
    exit(1);
}

void play(Snake * snake, Screen * screen,GameCondition * condition, Food * food)
{
	int WAIT_FOR_SECONDS = GetSnakeSpeed(snake) * 1000;

    while(!condition->quit)
    {
	    usleep(WAIT_FOR_SECONDS);
        int key = fetch_key();
        while (key != -1)
        {
            switch (key)
            {
                case 116:
                	condition->turbo = !condition->turbo;
			if (condition->turbo)
			{
				SetSnakeSpeed(snake, 8);
			}
			else
			{
				SetSnakeSpeed(snake, 80);
			}
			WAIT_FOR_SECONDS = GetSnakeSpeed(snake) * 1000;
			break;
                case 65:
			SetSnakeNextDirection(snake, DOWN);
                    	break;
                case 66:
			SetSnakeNextDirection(snake, UP);
                 	break;
                case 67:
			SetSnakeNextDirection(snake, RIGHT);
			break;
                case 68:
			SetSnakeNextDirection(snake, LEFT);
			break;
                case 112:
                    if (condition->paused)
                        condition->paused = 0;
                    else
                       condition->paused = 1;
                    break;
                case 120:
                    condition->quit = 2;
                    break;
            }
           
		key = fetch_key();
        }
       
	if (condition->quit)
            return;
        if (condition->paused)
            continue;
        
		int SNAKE_NEXT_DIRECTION = GetSnakeNextDirection(snake);
		int SNAKE_CUR_DIRECTION = GetSnakeDirection(snake);
		if (!(SNAKE_NEXT_DIRECTION == RIGHT && SNAKE_CUR_DIRECTION == LEFT) 
			  && !(SNAKE_NEXT_DIRECTION == LEFT && SNAKE_CUR_DIRECTION == RIGHT) 
			  && !(SNAKE_NEXT_DIRECTION == UP && SNAKE_CUR_DIRECTION == DOWN) 
			  && !(SNAKE_NEXT_DIRECTION == DOWN && SNAKE_CUR_DIRECTION == UP))
        { 
		   SetSnakeDirection(snake, SNAKE_NEXT_DIRECTION);
		}

		MoveSnakePos(snake);
		
		// game over when collpse with others.
		Pos head = (snake->pos)[0];
		Pos tail = (snake->pos)[GetSnakeSize(snake)];
		if (CheckSnakeCollisionWithoutHead(head, snake, screen))
		{
			game_over(screen,condition);
			return;
		}
		
		// food eat
		int respawn = 0;
		Pos foodPos = GetFoodPos( food );
		if (head.x == foodPos.x && head.y == foodPos.y)
		{
			int SNAKE_SIZE = GetSnakeSize(snake);
			
			SNAKE_SIZE += 1;
			SetSnakeSize(snake, SNAKE_SIZE);
			DrawGameInfo(snake, screen);
			(snake->pos)[SNAKE_SIZE].x = tail.x;
			(snake->pos)[SNAKE_SIZE].y = tail.y;
			respawn = 1;
		}
		// food drop
		if (respawn)
		{
			LocateFood( snake, food, screen );
		} 
		else 
		{
			MoveToPos( foodPos.x, foodPos.y);
			printf( "%c", GetFoodSymbol( food ) );
		}

		MoveToPos(screen->width, screen->height + 1);
		fflush(stdout);
    }
}

int rand_lim(int limit)
{
    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

   ///////////////////////////////// FOOD //////////////


int LocateFood(Snake * snake, Food * food, Screen * screen)
{
	int count;
	Pos pos;

	pos = GetRandomPos(screen->width, screen->height);
	count = 0;
	while ( CheckSnakeCollision(pos, snake, screen)
		    && count++ <= 50 )
	{
		pos = GetRandomPos(screen->width, screen->height);
	}
	SetFoodPos(food, pos);

	// Draw food
    	MoveToPos(pos.x, pos.y);
    	printf("%c", GetFoodSymbol(food));

	return 0;
}

////////////////////////////////////////////////////////

Pos GetRandomPos(int width, int height)
{	
	Pos pos;
	pos.x = rand_lim(width);
	pos.y = rand_lim(height);
	return pos;
}

