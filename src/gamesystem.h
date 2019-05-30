/*
 * 전체적인 게임진행을 담당하는
 * 조건 초기화, 게임시작, 게임오버, 키보드 입력, 뱀의 충돌과
 * 음식의 세팅 함수를 포함하는 헤더. 
 *
 * 음식의 정보인 food.h와 출력을 담당하는 draw.h를 포함하고 있다.
 */

#ifndef __GAMESYSTEM_H_
#define __GAMESYSTEM_H_

#include "draw.h"
#include "food.h"

/*게임의 상태와 옵션을 나타낸다.*/
typedef struct GameCondition
{
	int turbo;
	int paused;
	int quit;

}GameCondition;


void InitGameCondition( GameCondition * condition );
void NewGame( Snake * snake, Screen * screen, Food * food );
void PrintGameOver( Screen * screen, GameCondition * condition );
int FetchKey();
void Fatal( char * reason );
void Play( Snake * snake, Screen * screen,GameCondition * condition, Food * food );
int RandLim( int limit );
Pos GetRandomPos( int width, int height );
int LocateFood( Snake * snake, Food * food, Screen * screen );


#endif
