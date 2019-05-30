#ifndef __FOOD_H_
#define __FOOD_H_

#include "location.h"

typedef struct Food
{
	char DISPLAY_FOOD;
	Pos pos;
}Food;

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

#endif
