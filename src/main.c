#include <stdio.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include "location.h"
#include <stdbool.h>

#define MAX_SIZE   800
#define MAP_MAX_WIDTH  300
#define MAP_MAX_HEIGHT 300

const enum Dir {LEFT, RIGHT, UP, DOWN};

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

typedef struct Food
{
	char DISPLAY_FOOD;
	Pos pos;
}Food;

typedef struct Screen
{
	int width;
	int height;

}Screen;

typedef struct GameCondition
{
	int turbo;
	int paused;
	int quit;

}GameCondition;

// Snake Functions
void InitSnake(Snake * snake);

int SetSnakeSize(Snake * snake, int size_);
int GetSnakeSize(Snake * snake);
int SetSnakeDirection(Snake * snake, int direction_);
int GetSnakeDirection(Snake * snake);
int SetSnakeNextDirection(Snake * snake, int nextDirection_);
int GetSnakeNextDirection(Snake * snake);
int SetSnakeSpeed(Snake * snake, int speed_);
int GetSnakeSpeed(Snake * snake);

int CheckSnakeCollision(Pos px, Snake * snake, Screen * screen);
int CheckSnakeCollisionWithoutHead(Pos px, Snake * snake, Screen * screen);
int SetSnakeInitPos(Snake * snake, const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
int MoveSnakePos(Snake * snake);

// Food Functions
void InitFood(Food * food);
char GetFoodSymbol(Food * food);

int SetFoodPos(Food * food, Pos pos_);
Pos GetFoodPos(Food * food);



// Game System
Pos GetRandomPos(int width, int height);
int LocateFood( Snake * snake, Food * food, Screen * screen );

// Display
void InitScreen(Screen * screen, struct winsize * w);
void Clear();
void MoveToPos(int x, int y);
void draw_snake(Snake * snake);
void DrawGameInfo(Snake * snake, Screen * screen);

struct termios orig_term_attr;
struct termios new_term_attr;

char arena[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];

int rand_lim(int limit)
{
    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
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

void draw_map(Screen * screen)
{
    int x,y;
    x = 0;
    while (x-1 < screen->width)
    {
        y = 0;
        do {
            if (x == 0 || y == 0 || x >= screen->width || y >= screen->height-1)
            {
                arena[x][y] = '*';
                MoveToPos(x,y);
                printf("*");
            }
        } while (y++ < screen->height-1);
       x++;
    }
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

void InitGameCondition(GameCondition * condition)
{
	condition->paused = 0;
	condition->quit = 0;
	condition->turbo = 0;
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


int SetSnakeSize(Snake * snake, int size_)
{
	snake->size = size_;
}

int GetSnakeSize(Snake * snake)
{
	return snake->size;
}

int CheckSnakeCollision(Pos px, Snake * snake, Screen * screen)
{
	const int SNAKE_SIZE = GetSnakeSize(snake);

	if (arena[px.x][px.y] != ' ')
	   return 1;
	if (px.x < 1)
	   return 1;
	if (px.y < 1)
	   return 1;
	if (px.x > screen->width - 2)
	   return 1;
	if (px.y > screen->height - 1)
		return 1;


	for ( int i = 0; i < SNAKE_SIZE; i++ )
	{
		if ( snake->pos[i].x == px.x
			  && snake->pos[i].y == px.y )
		{
			return 1;
		}
	}

	return 0;
}

int CheckSnakeCollisionWithoutHead(Pos px, Snake * snake, Screen * screen)
{
	const int SNAKE_SIZE = GetSnakeSize(snake);

	if (arena[px.x][px.y] != ' ')
	   return 1;
	if (px.x < 1)
	   return 1;
	if (px.y < 1)
	   return 1;
	if (px.x > screen->width - 2)
	   return 1;
	if (px.y > screen->height - 1)
		return 1;

	for ( int i = 1; i < SNAKE_SIZE; i++ )
	{
		
		if ( snake->pos[i].x == px.x
			  && snake->pos[i].y == px.y )
		{
			return 1;
		}
	}

	return 0;
}
int SetSnakeInitPos(Snake * snake, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
	const int SNAKE_SIZE = GetSnakeSize(snake);
	int start_x = SCREEN_WIDTH / 2;
	int start_y = SCREEN_HEIGHT / 2;
	
	for ( int i = 0; i < SNAKE_SIZE; i++)
	{
		if ( start_x < 0 )
		{
			fatal("The screen is too small.");
		}
		snake->pos[i].x = start_x--;
		snake->pos[i].y = start_y;
	}
}

int MoveSnakePos(Snake * snake)
{
   // Moves snake into a pos that snake is looking at
   // return: when snake moves without erros, returns true.
   // return: when snake can't move, return false.
	const int SNAKE_SIZE = GetSnakeSize(snake);
	const int SNAKE_DIRECTION = GetSnakeDirection(snake);
	int add_x = 0;
	int add_y = 0;
	

	switch (SNAKE_DIRECTION)
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

	Pos head = (snake->pos)[0];
	Pos tail = (snake->pos)[SNAKE_SIZE];

	MoveToPos(tail.x, tail.y);
	printf(" ");

	MoveToPos(head.x, head.y);
	printf("%c", snake->SnakeChar);

	// why it is not snakesize -1
	for ( int i = SNAKE_SIZE; i > 0; i-- )
	{
		snake->pos[i].x = snake->pos[i-1].x;
		snake->pos[i].y = snake->pos[i-1].y;
	}

	head.x += add_x;
	head.y += add_y;

	(snake->pos)[0].x += add_x;
	(snake->pos)[0].y += add_y;

	MoveToPos((snake->pos)[0].x, (snake->pos)[0].y);
	printf("%c", snake->SnakeChar_First);
	
	MoveToPos((snake->pos)[SNAKE_SIZE].x, (snake->pos)[SNAKE_SIZE].y);
	printf("%c", snake->SnakeChar_Tail);
	
	return 0;
}


void InitSnake(Snake * snake)
{
	snake->SnakeChar_First = 'O';
	snake->SnakeChar = 'o';
	snake->SnakeChar_Tail = '.';
	
	for ( int i = 0; i < MAX_SIZE; i++ )
	{
		(snake->pos)[i].x = -10;
		(snake->pos)[i].y = -10;
	}
	
	SetSnakeSize(snake, 3);
	SetSnakeSpeed(snake, 80);
	SetSnakeNextDirection(snake, RIGHT);
	SetSnakeDirection(snake, RIGHT);
}


void draw_snake(Snake * snake)
{
	const int SNAKE_SIZE = GetSnakeSize(snake); 
    int remaining = -1;

	while (remaining++ < SNAKE_SIZE)
    {
        char symbol = snake->SnakeChar;
        if (remaining == 0)
            symbol = snake->SnakeChar_First;
        else if (remaining == SNAKE_SIZE)
            symbol = snake->SnakeChar_Tail;
		
		if (snake->pos[remaining].x < 0 || snake->pos[remaining].y < 0) break;
        
		MoveToPos((snake->pos)[remaining].x, (snake->pos)[remaining].y);
        printf("%c", symbol);
    }

    fflush(stdout);

}

void DrawGameInfo(Snake * snake, Screen * screen)
{
	const int SCREEN_HEIGHT = 31;
	const int SNAKE_SIZE = GetSnakeSize(snake);
	const int SCORES = SNAKE_SIZE - 3;

    MoveToPos(0, SCREEN_HEIGHT ); 

	printf("Score: %i | t - turbo | x - Quit the game | p - pause the game ", SCORES);
}

int SetSnakeDirection(Snake * snake, int direction_)
{
	if (direction_ != RIGHT
		&& direction_ != LEFT
		&& direction_ != UP
		&& direction_ != DOWN)
	{
	   return -1;
	}

	snake->direction = direction_;
	return 0;
}

int GetSnakeDirection(Snake * snake)
{
	return snake->direction;
}

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

int SetSnakeNextDirection(Snake * snake, int nextDirection_)
{
	snake->new_direction = nextDirection_;
	return 0;
}

int GetSnakeNextDirection(Snake * snake)
{
	return snake->new_direction;
}

int SetSnakeSpeed(Snake * snake, int speed_)
{
	if (speed_ < 0)
	{
		return -1;
	}

	snake->speed = speed_;
	return 0;
}

int GetSnakeSpeed(Snake * snake)
{
	return snake->speed;
}

   ///////////////////////////////// FOOD //////////////

void InitFood(Food * food)
{
	food->DISPLAY_FOOD = '@';	
}

char GetFoodSymbol(Food * food)
{
	return food->DISPLAY_FOOD;
}

int SetFoodPos(Food * food, Pos pos_)
{
	food->pos = pos_;
}
Pos GetFoodPos(Food * food)
{
	return food->pos;
}

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


void Clear()
{
	printf("\033[H\033[J");
}


void MoveToPos(int x, int y)
{
	printf("\033[%d;%dH",(y+1),(x+1));
}
