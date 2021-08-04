// TODO:

#include "vector.h"
#include "blocktexture.h"

#pragma once

enum blocktype_t {
	AIR = 0,
	STONE = 1,
	DIRT = 2,
	GRASS = 3,
};

enum blockface_t {	
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,

	UP = 4,
	DOWN = 5
};

class Block {
	public:
		Block();

		blocktype_t blockType = blocktype_t::AIR;

		BlockTexture GetSideTexture(blockface_t side);
};