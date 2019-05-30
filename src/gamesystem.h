#ifndef __GAMESYSTEM_H_
#define __GAMESYSTEM_H_

#include "draw.h"
#include "food.h"

typedef struct GameCondition
{
	int turbo;
	int paused;
	int quit;

}GameCondition;


void InitGameCondition( GameCondition * condition );
void NewGame( Snake * snake, Screen * screen, Food * food );
void GameOver( Screen * screen, GameCondition * condition );
int FetchKey();
void Fatal( char * reason );
void Play( Snake * snake, Screen * screen,GameCondition * condition, Food * food );
int RandLim( int limit );
Pos GetRandomPos( int width, int height );
int LocateFood( Snake * snake, Food * food, Screen * screen );


#endif
