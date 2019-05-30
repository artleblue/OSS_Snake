#include <stdio.h>
#include <stdlib.h>
#include "gamesystem.h"


void InitGameCondition( GameCondition * condition )
{
	condition->paused = 0;
	condition->quit = 0;
	condition->turbo = 0;
}

/*새로운 게임을 세팅하는 함수*/
void NewGame( Snake * snake, Screen * screen, Food * food )
{
	int ax = 0;
   int ay = 0;


   SetSnakeSize( snake, 3 );
	InitFood( food );
	
	/*맵의 공백부분을 채워넣는다.*/
	do
	{
		ay = 0;

      do
		{
			arena[ax][ay] = ' ';

      } while ( ++ay < MAP_MAX_WIDTH );

	} while ( ++ax < MAP_MAX_HEIGHT );
  
	for ( int i = 0 ; i < MAX_SIZE ; i++ )
	{
		snake->pos[i].x = -10;
		snake->pos[i].y = -10;
	}

	SetSnakeInitPos( snake, screen->width, screen->height ); 

   Clear();
	
   DrawMap( screen );

   LocateFood( snake, food, screen );
	
	/*뱀과 게임정보를 출력한다.*/
   DrawGameInfo( snake, screen );
	DrawSnake( snake );
}

void PrintGameOver( Screen * screen, GameCondition * condition )
{
    int key = -1;


    MoveToPos( screen->width / 2, screen->height / 2 );
    printf( "* GAME OVER *" );
    MoveToPos( screen->width / 2 - 10, screen->height / 2 + 2 );
    printf( "Press x to exit or n for new game" );

    while ( key != 110 && key != 120 )
    {
        key = FetchKey();
        usleep(200);
    }

    if ( key == 120 )
	 {   
		 condition->quit = 2;
	 }
    else
	 {  
		 condition->quit = 1;
	 }
}

/*키보드 입력을 받아오는 함수.*/
int FetchKey()
{
    return fgetc(stdin);
}

/*오류 발생 시, 오류원인을 출력하는 함수.*/
void Fatal( char * reason )
{
    printf( "FATAL: %s\n", reason );
    exit(1);
}

/*뱀의 이동, 옵션 적용, 음식의 세팅 등의 게임플레이를 진행하는 함수.*/
void Play( Snake * snake, Screen * screen, GameCondition * condition, Food * food )
{
	int WAIT_FOR_SECONDS = GetSnakeSpeed( snake ) * 1000; //입력 시간간격을 설정한다.
	
	
	/*입력 사이에 시간간격을 두고 키보드 입력을 받는다.*/
	while( !condition->quit )
   { 
		usleep( WAIT_FOR_SECONDS ); // 간격만큼 대기한다.

      int key = FetchKey(); // 키보드입력
		
		/*입력된 키에 따라 기능을 구분한다.*/
      while ( key != -1 )
      {
			switch ( key )
         {
				case 116: // 터보 기능의 on off

            	condition->turbo = !condition->turbo;

					if ( condition->turbo )
					{
						SetSnakeSpeed( snake, 8 );
					}
					else
					{
						SetSnakeSpeed( snake, 80 );
					}

					WAIT_FOR_SECONDS = GetSnakeSpeed( snake ) * 1000; // 입력 시간간격 재설정
					break;

				case 65: // 뱀이 아래쪽으로 향함

					SetSnakeNextDirection( snake, DOWN );
              	break;

            case 66: // 뱀이 위쪽으로 향함

					SetSnakeNextDirection( snake, UP );
              	break;
           
				case 67: // 뱀이 오른쪽으로 향함

					SetSnakeNextDirection( snake, RIGHT );
					break;

            case 68: // 뱀이 왼쪽으로 향함
					
					SetSnakeNextDirection( snake, LEFT );
					break;

           	case 112: // 게임 일시정지상태로 전환 / 해제

               if ( condition->paused )
               {
						condition->paused = 0;
					}
               else
					{
						condition->paused = 1;
                  break;
					}

            case 120: // 게임종료상태로 전환

               condition->quit = 2;
               break;
            }
           
			key = FetchKey(); // 키보드입력
		}
		
		if ( condition->quit ) // 게임강제종료
      {
			return;
		}
		
      if ( condition->paused ) // 게임 일시정지
		{     
			continue;
		}
      
		/*다음 뱀의 방향을 설정한다.*/
		int SNAKE_NEXT_DIRECTION = GetSnakeNextDirection(snake);
		int SNAKE_CUR_DIRECTION = GetSnakeDirection(snake);

		
		/*뱀의 방향과 입력된 방향이 반대라면 키의 입력을 무효화하고
		 진행방향을 바꾸지 않는다.
		 */
		if ( !(SNAKE_NEXT_DIRECTION == RIGHT && SNAKE_CUR_DIRECTION == LEFT) 
				&& !(SNAKE_NEXT_DIRECTION == LEFT && SNAKE_CUR_DIRECTION == RIGHT) 
		 		&& !(SNAKE_NEXT_DIRECTION == UP && SNAKE_CUR_DIRECTION == DOWN) 
		 		&& !(SNAKE_NEXT_DIRECTION == DOWN && SNAKE_CUR_DIRECTION == UP) )
      { 
			SetSnakeDirection( snake, SNAKE_NEXT_DIRECTION );
		}

		MoveSnakePos( snake );
		
		Pos head = snake->pos[0];
		Pos tail = snake->pos[GetSnakeSize(snake)];
		
		/*뱀이 자기자신과 충돌하였는지 검사한다.*/
		if ( CheckSnakeCollisionWithoutHead( head, snake, screen ) )
		{
			PrintGameOver( screen,condition );
			return;
		}
			
		int respawn = 0; // 음식을 생성해야함을 나타내는 변수


		Pos foodPos = GetFoodPos( food ); // 음식의 위치 받아오기
		
		/*뱀이 음식을 먹었는지 확인한다.*/
		if ( head.x == foodPos.x && head.y == foodPos.y )
		{
			int SNAKE_SIZE = GetSnakeSize( snake );
			
			SNAKE_SIZE += 1;

			SetSnakeSize( snake, SNAKE_SIZE );
			DrawGameInfo( snake, screen );
			
			snake->pos[SNAKE_SIZE].x = tail.x;
			snake->pos[SNAKE_SIZE].y = tail.y;
			
			respawn = 1; // 음식을 생성해야함
		}
		
		if ( respawn )
		{
			LocateFood( snake, food, screen ); // 음식 재배치
		} 
		else 
		{
			/*음식을 현재 위치에 재출력*/
			MoveToPos( foodPos.x, foodPos.y);
			printf( "%c", GetFoodSymbol( food ) );
		}

		MoveToPos( screen->width, screen->height + 1 );
		
		fflush( stdout );
    }
}

/*화면의 크기에 따른 랜덤의 범위를 지정하고
  범위 내에서 랜덤 숫자를 반환하는 함수.
 
 인자로 너비나 높이를 받는다.
 */
int RandLim( int limit )
{
	int divisor = RAND_MAX / ( limit + 1 );
   int retval;


   do
	{
		retval = rand() / divisor;

   }while ( retval > limit );

   return retval;
}

/*랜덤 위치를 받아오는 함수.*/
Pos GetRandomPos( int width, int height )
{
	Pos pos;

	pos.x = RandLim( width );
	pos.y = RandLim( height );

	return pos;
}

/*음식을 랜덤한 위치에 자리시키는 함수*/
int LocateFood( Snake * snake, Food * food, Screen * screen )
{
	int count;
	Pos pos;

	pos = GetRandomPos( screen->width, screen->height );
	count = 0;
	
	while ( CheckSnakeCollision( pos, snake, screen )
			&& count++ <= 50 )
	{
		pos = GetRandomPos( screen->width, screen->height );
	}

	SetFoodPos( food, pos );

   MoveToPos( pos.x, pos.y );
   printf("%c", GetFoodSymbol( food ) );

	return 0;
}

