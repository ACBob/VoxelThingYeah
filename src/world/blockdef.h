// Holds all the block definitions
#ifndef BLOCKDEF_H
#define BLOCKDEF_H

// size of X,Y,Z of blocks
#define BLOCKUNIT 1.0f

enum blocktype_t {
	AIR      = 0,
	STONE    = 1,
	DIRT     = 2,
	GRASS    = 3,
	COBBLE   = 4,
	PLANKS   = 5,
	BEDROCK  = 6
};

enum blockmaterial_t {
	MAT_NONE     = 0,
	MAT_STONE    = 1,
	MAT_LOOSE    = 2,
	MAT_WOOD     = 3
};

static blockmaterial_t GetBlockMaterial(blocktype_t blockType)
{
	switch(blockType)
	{
		default:
		case STONE:
		case COBBLE:
		case BEDROCK:
			return blockmaterial_t::MAT_STONE;
		break;

		case DIRT:
		case GRASS:
			return blockmaterial_t::MAT_LOOSE;
		break;

		case PLANKS:
			return blockmaterial_t::MAT_WOOD;
		break;
	}
}

#endif