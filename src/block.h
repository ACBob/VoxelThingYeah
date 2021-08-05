// TODO:

#include "direction.h"

#include "vector.h"
#include "blocktexture.h"

#pragma once

enum blocktype_t {
	AIR = 0,
	STONE = 1,
	DIRT = 2,
	GRASS = 3,
};

class Block {
	public:
		Block();

		blocktype_t blockType = blocktype_t::AIR;

		BlockTexture GetSideTexture(Direction side);
};