#ifndef __FOOD_H_
#define __FOOD_H_

#include "location.h"


typedef struct Food
{
	char DISPLAY_FOOD;
	Pos pos;
}Food;


void InitFood( Food * food );
char GetFoodSymbol( Food * food );
void SetFoodPos( Food * food, Pos pos_ );
Pos GetFoodPos( Food * food );

#endif
