#include <stdio.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_SIZE   800
#define MAP_MAX_WIDTH  300
#define MAP_MAX_HEIGHT 300


const enum Dir {LEFT,RIGHT,UP,DOWN};

typedef struct Position
{
    int x;
    int y;

}Pos;

typedef struct Snake
{
	int size;
	int speed;
	int direction;
	int new_direction;

}Snake;

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

const char SnakeChar_First = 'O';
const char SnakeChar = 'o';
const char SnakeChar_Tail = '.';
const char food = '@';

Pos pos[MAX_SIZE];
Pos food_position;

struct termios orig_term_attr;
struct termios new_term_attr;

char arena[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];


void Clear()
{
	printf("\033[H\033[J");
}

void MoveToPos(int x, int y)
{
	printf("\033[%d;%dH",(y+1),(x+1));
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

int fetch_key()
{
    return fgetc(stdin);
}

void fatal(char *reason)
{
    printf("FATAL: %s\n", reason);
    exit(1);
}

void randomize_food(Screen * screen)
{
    food_position.x = rand_lim(screen->width);
    food_position.y = rand_lim(screen->height);
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

void hud(Snake * snake)
{
	 const int SCREEN_HEIGHT = 31;
    MoveToPos(0, SCREEN_HEIGHT ); 

	 printf("Score: %i | t - turbo | x - Quit the game | p - pause the game ", snake->size - 3);
}

int check_snake_collision(Pos px, Snake * snake, Screen * screen)
{
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

    int remaining = -1;
    while (remaining++ < snake->size)
    {
        if (pos[remaining].x == px.x && pos[remaining].y == px.y)
            return 1;
    }

    return 0;
}

void generate_food(Snake * snake, Screen * screen)
{
    randomize_food(screen);
    while (check_snake_collision(food_position,snake,screen))
        randomize_food(screen);
    // draw it
    MoveToPos(food_position.x, food_position.y);
    printf("%c", food);
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

void move_snake(Snake * snake, Screen * screen, GameCondition * condition)
{
    int add_x = 0;
    int add_y = 0;
    switch (snake->direction)
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

    Pos head = pos[0];
    Pos tail = pos[snake->size];

    MoveToPos(tail.x, tail.y);
    printf(" ");

    MoveToPos(head.x, head.y);
    printf("%c", SnakeChar);

    int remaining = snake->size + 1;
   
	 while (--remaining > 0)
    {
        pos[remaining].x = pos[remaining-1].x;
        pos[remaining].y = pos[remaining-1].y;
    }

    head.x += add_x;
    head.y += add_y;

    int respawn = 0;

    if (check_snake_collision(head,snake,screen))
    {
        game_over(screen,condition);
        return;
    }

    if (head.x == food_position.x && head.y == food_position.y)
    {
        snake->size++;
        hud(snake);
        pos[snake->size].x = tail.x;
        pos[snake->size].y = tail.y;
        respawn = 1;
    }

    pos[0].x += add_x;
    pos[0].y += add_y;

    MoveToPos(pos[0].x, pos[0].y);
    printf("%c", SnakeChar_First);

    MoveToPos(pos[snake->size].x, pos[snake->size].y);
    printf("%c", SnakeChar_Tail);
    
	 
	 if (respawn)
        generate_food(snake,screen);
    else
    {
        MoveToPos(food_position.x, food_position.y);
        printf("%c", food);
    }
    
	 MoveToPos(screen->width, screen->height + 1);
    fflush(stdout);
}

void GenerateSnake(Snake * snake, Screen * screen)
{
	int start_x = screen->width / 2;
	int start_y = screen->height / 2;
	int remaining = -1;
	
	while (remaining++ < snake->size)
	{
		if(start_x < 0)
			fatal("The screen is too small");
		pos[remaining].x = start_x--;
		pos[remaining].y = start_y;
	}
}

void draw_snake(Snake * snake)
{
    int remaining = -1;
   
	 while (remaining++ < snake->size)
    {
        char symbol = SnakeChar;
        if (remaining == 0)
            symbol = SnakeChar_First;
        else if (remaining == snake->size)
            symbol = SnakeChar_Tail;
       
        MoveToPos(pos[remaining].x, pos[remaining].y);
        printf("%c", symbol);
    }
    fflush(stdout);
}

void play(Snake * snake, Screen * screen,GameCondition * condition)
{
    while(!condition->quit)
    {
        if (!condition->turbo)
            usleep(snake->speed * 1000);
        else
            usleep(snake->speed * 100);
        int key = fetch_key();
        while (key != -1)
        {
            switch (key)
            {
                case 116:
                    condition->turbo = !condition->turbo;
                    break;
                case 65:
                    snake->new_direction = DOWN;
                    break;
                case 66:
                    snake->new_direction = UP;
                    break;
                case 67:
                    snake->new_direction = RIGHT;
                    break;
                case 68:
                    snake->new_direction = LEFT;
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
        
		  if (!(snake->new_direction == RIGHT && snake->direction == LEFT) && !(snake->new_direction == LEFT && snake->direction == RIGHT) &&
            !(snake->new_direction == UP && snake->direction == DOWN) && !(snake->new_direction == DOWN && snake->direction == UP))
            snake->direction = snake->new_direction;
        move_snake(snake, screen,condition);
    }
}


void new_game(Snake * snake, Screen * screen)
{
    snake->size = 3;
   
    int ax = 0;
    int ay = 0;
    do
    {
        ay = 0;
        do
        {
            arena[ax][ay] = ' ';
        } while (++ay < MAP_MAX_WIDTH);
    } while (++ax < MAP_MAX_HEIGHT);
   
	 int current_pos = -1;
    
	 while (current_pos++ < MAX_SIZE)
    {
        pos[current_pos].x = -10;
        pos[current_pos].y = -10;
    }
    
	 GenerateSnake(snake, screen);

    Clear();
    draw_map(screen);
    generate_food(snake, screen);
    hud(snake);
    draw_snake(snake);
}

void InitSnake(Snake * snake)
{
	snake->size = 0;
	snake->speed = 80;
	snake->new_direction = RIGHT;
	snake->direction = RIGHT;
}

void InitGameCondition(GameCondition * condition)
{
	condition->paused = 0;
	condition->quit = 0;
	condition->turbo = 0;
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
		screen->height = MAP_MAX_HEIGHT - 1;
	}
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

    struct winsize w;

    srand(time(NULL));

	 InitSnake(&snake);
	 InitGameCondition(&condition);
    
	 SetTerminal(&w);

	 InitScreen(&screen, &w);
 
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
 
	 while (condition.quit != 2)
    {
        new_game(&snake,&screen);
        condition.quit = 0;
        play(&snake,&screen,&condition);
    }

    Clear();
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
    
	 return 0;
}
