#include <stdio.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_SIZE   800
#define MAX_WIDTH  300
#define MAX_HEIGHT 300

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

}Snake;

char SnakeChar_First = 'O';
char SnakeChar = 'o';
char SnakeChar_Tail = '.';

char food = '@';

int screen_width = -30;
int screen_height = -30;

int direction;
int new_direction;

Snake snake;

struct Position pos[MAX_SIZE];
struct Position food_position;

struct termios orig_term_attr;
struct termios new_term_attr;

int turbo = 0;
int paused = 0;
int quit = 0;
char arena[MAX_HEIGHT][MAX_WIDTH];

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
    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    return fgetc(stdin);
}

void fatal(char *reason)
{
    printf("FATAL: %s\n", reason);
    exit(1);
}

void randomize_food()
{
    food_position.x = rand_lim(screen_width);
    food_position.y = rand_lim(screen_height);
}

void draw_map()
{
    int x,y;
    x = 0;
    while (x-1 < screen_width)
    {
        y = 0;
        do {
            if (x == 0 || y == 0 || x >= screen_width || y >= screen_height-1)
            {
                arena[x][y] = '*';
                MoveToPos(x,y);
                printf("*");
            }
        } while (y++ < screen_height-1);
       x++;
    }
}

void hud()
{
	 const int SCREEN_HEIGHT = 31;
    MoveToPos(0, SCREEN_HEIGHT ); 

	 printf("Score: %i | t - turbo | x - Quit the game | p - pause the game ", snake.size - 3);
}

int check_snake_collision(struct Position px)
{
    if (arena[px.x][px.y] != ' ')
        return 1;
    if (px.x < 1)
        return 1;
    if (px.y < 1)
        return 1;
    if (px.x > screen_width - 2)
        return 1;
    if (px.y > screen_height - 1)
        return 1;

    int remaining = -1;
    while (remaining++ < snake.size)
    {
        if (pos[remaining].x == px.x && pos[remaining].y == px.y)
            return 1;
    }

    return 0;
}

void generate_food()
{
    randomize_food();
    while (check_snake_collision(food_position))
        randomize_food();
    // draw it
    MoveToPos(food_position.x, food_position.y);
    printf("%c", food);
}

void game_over()
{
    MoveToPos(screen_width / 2, screen_height / 2);
    printf("* GAME OVER *");
    MoveToPos((screen_width / 2) - 10, (screen_height / 2) + 2);
    printf("Press x to exit or n for new game");
    int key = -1;
    while (key != 110 && key != 120)
    {
        key = fetch_key();
        usleep(200);
    }
    if (key == 120)
        quit = 2;
    else
        quit = 1;
}

void move_snake()
{
    int add_x = 0;
    int add_y = 0;
    switch (direction)
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
    Pos tail = pos[snake.size];

    // replace tail with space
    MoveToPos(tail.x, tail.y);
    printf(" ");

    // replace head with normal
    MoveToPos(head.x, head.y);
    printf("%c", SnakeChar);

    // shift the snake
    int remaining = snake.size + 1;
    while (--remaining > 0)
    {
        pos[remaining].x = pos[remaining-1].x;
        pos[remaining].y = pos[remaining-1].y;
    }

    head.x += add_x;
    head.y += add_y;

    int respawn = 0;

    if (check_snake_collision(head))
    {
        game_over();
        return;
    }

    // check if we ate food
    if (head.x == food_position.x && head.y == food_position.y)
    {
        snake.size++;
        hud();
        pos[snake.size].x = tail.x;
        pos[snake.size].y = tail.y;
        respawn = 1;
    }

    pos[0].x += add_x;
    pos[0].y += add_y;

    // make new head
    MoveToPos(pos[0].x, pos[0].y);
    printf("%c", SnakeChar_First);
    // make new tail
    MoveToPos(pos[snake.size].x, pos[snake.size].y);
    printf("%c", SnakeChar_Tail);
    if (respawn)
        generate_food();
    else
    {
        MoveToPos(food_position.x, food_position.y);
        printf("%c", food);
    }
    MoveToPos(screen_width, screen_height + 1);
    fflush(stdout);
}

void GenerateSnake()
{
	//create new snake in middle of screen
	int start_x = screen_width / 2;
	int start_y = screen_height / 2;
	//generate snake
	int remaining = -1;
	while (remaining++ < snake.size)
	{
		if(start_x < 0)
			fatal("The screen is too small");
		pos[remaining].x = start_x--;
		pos[remaining].y = start_y;
	}
}

void draw_snake()
{
    int remaining = -1;
    while (remaining++ < snake.size)
    {
        char symbol = SnakeChar;
        if (remaining == 0)
            symbol = SnakeChar_First;
        else if (remaining == snake.size)
            symbol = SnakeChar_Tail;
        // go to position
        MoveToPos(pos[remaining].x, pos[remaining].y);
        printf("%c", symbol);
    }
    fflush(stdout);
}

void play()
{
    while(!quit)
    {
        if (!turbo)
            usleep(snake.speed * 1000);
        else
            usleep(snake.speed * 100);
        int key = fetch_key();
        while (key != -1)
        {
            switch (key)
            {
                case 116:
                    turbo = !turbo;
                    break;
                case 65:
                    new_direction = DOWN;
                    break;
                case 66:
                    new_direction = UP;
                    break;
                case 67:
                    new_direction = RIGHT;
                    break;
                case 68:
                    new_direction = LEFT;
                    break;
                case 112:
                    if (paused)
                        paused = 0;
                    else
                        paused = 1;
                    break;
                case 120:
                    quit = 2;
                    break;
            }
            key = fetch_key();
        }
        if (quit)
            return;
        if (paused)
            continue;
        if (!(new_direction == RIGHT && direction == LEFT) && !(new_direction == LEFT && direction == RIGHT) &&
            !(new_direction == UP && direction == DOWN) && !(new_direction == DOWN && direction == UP))
            direction = new_direction;
        move_snake();
    }
}


void new_game()
{
    snake.size = 3;
    // reset for debugging
    int ax = 0;
    int ay = 0;
    do
    {
        ay = 0;
        do
        {
            arena[ax][ay] = ' ';
        } while (++ay < MAX_WIDTH);
    } while (++ax < MAX_HEIGHT);
    int current_pos = -1;
    while (current_pos++ < MAX_SIZE)
    {
        pos[current_pos].x = -10;
        pos[current_pos].y = -10;
    }
    
	 GenerateSnake();

    new_direction =   RIGHT;
    direction =       RIGHT;
    Clear();
    draw_map();
    generate_food();
    hud();
    draw_snake();
}

void SnakeInit()
{
	snake.size = 0;
	snake.speed = 80;
}


int main(int argc, char **argv)
{
    struct winsize w;
    srand(time(NULL));

	 SnakeInit();

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (screen_width < 0 || screen_height < 0)
    {
        screen_height = w.ws_row - 1;
        screen_width = w.ws_col;
    }
    if (screen_width >= MAX_WIDTH)
        screen_width = MAX_WIDTH - 1;
    if (screen_height >= MAX_HEIGHT)
        screen_height = MAX_HEIGHT - 1; 
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
    while (quit != 2)
    {
        new_game();
        quit = 0;
        play();
    }
    /* restore the original terminal attributes */
    Clear();
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
    return 0;
}
