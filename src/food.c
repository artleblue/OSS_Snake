#include <stdio.h>
#include "food.h"


void InitFood( Food * food )
{
	food->DISPLAY_FOOD = '@';	
}

char GetFoodSymbol( Food * food )
{
	return food->DISPLAY_FOOD;
}

void SetFoodPos( Food * food, Pos pos_ )
{
	food->pos = pos_;
}

Pos GetFoodPos( Food * food )
{
	return food->pos;
}
