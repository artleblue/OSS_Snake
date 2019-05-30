/*
 * 위치 정보와 방향정보를 포함하는 헤더.
 */

#ifndef __LOCATION_H_
#define __LOCATION_H_

typedef struct Position
{
	int x;
	int y;
}Pos;

const enum Dir {LEFT, RIGHT, UP, DOWN};

#endif
