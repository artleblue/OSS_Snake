/*
* 음식에 대한 정보와 음식의 위치와 기호에 관여하는 함수를 포함하는 헤더.
*
* 위치의 정보인 location.h를 포함하고 있다.
*/

#ifndef __FOOD_H_
#define __FOOD_H_

#include "location.h"


typedef struct Food
{
	char DISPLAY_FOOD; //음식을 나타내는 기호
	Pos pos; //음식 위치
}Food;


void InitFood( Food * food );
char GetFoodSymbol( Food * food );
void SetFoodPos( Food * food, Pos pos_ );
Pos GetFoodPos( Food * food );

#endif
