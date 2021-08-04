// TODO:

#include "vector.h"
#include "blocktexture.h"

#pragma once

enum blocktype_t {
	AIR = 0,
	STONE = 1,
	DIRT = 2,
	GRASS = 3
};

enum blockface_t {	
	NORTH = 1,
	EAST = 2,
	SOUTH = 3,
	WEST = 4,

	UP = 5,
	DOWN = 6
};

class Block {
	public:
		Block();

		blocktype_t blockType = blocktype_t::AIR;

		BlockTexture GetSideTexture(blockface_t side);
};