/*
*뱀, 맵, 게임 정보를 출력하는 함수를 포함하는 헤더.
*
*뱀의 정보인 snake.h를 포함하고 있다.
*/

#ifndef __DRAW_H_
#define __DRAW_H_

#include "snake.h"


void DrawSnake( Snake * snake );
void DrawGameInfo( Snake * snake, Screen * screen );
void DrawMap( Screen * screen );

#endif
