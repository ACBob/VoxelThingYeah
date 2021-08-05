#pragma once

#include "vector.h"

enum Direction {	
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,

	UP = 4,
	DOWN = 5
};

const Vector DirectionVector[] = {
	{0, 0, 1}, // NORTH
	{1, 0, 0}, // EAST
	{0, 0, -1}, // SOUTH
	{-1, 0, 0}, // WEST

	{0, 1, 0}, // UP
	{0, -1, 0} // DOWN
};