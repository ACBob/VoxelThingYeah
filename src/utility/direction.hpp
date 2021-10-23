#pragma once

#include "vector.hpp"

enum Direction {
	NORTH = 0, // +Z
	EAST  = 1, // -X
	SOUTH = 2, // -Z
	WEST  = 3, // +X

	UP	 = 4, // +Y
	DOWN = 5  // -Y
};

static const CVector DirectionVector[] = {
	{ 0, 0, 1 },  // NORTH
	{ 1, 0, 0 },  // EAST
	{ 0, 0, -1 }, // SOUTH
	{ -1, 0, 0 }, // WEST

	{ 0, 1, 0 }, // UP
	{ 0, -1, 0 } // DOWN
};