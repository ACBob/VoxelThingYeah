// Holds fancy block definition stuffy stuff
// size of X,Y,Z of blocks
#define BLOCKUNIT 1.0f

#pragma once

enum blocktype_t {
	AIR      = 0,
	STONE    = 1,
	DIRT     = 2,
	GRASS    = 3,
	COBBLE   = 4,
	PLANKS   = 5,
	BEDROCK  = 6,
	GLASS    = 7,
	LOG      = 8,
	LEAVES   = 9,
};

enum blockmaterial_t {
	MAT_NONE     = 0,
	MAT_STONE    = 1,
	MAT_LOOSE    = 2,
	MAT_WOOD     = 3,
	MAT_GLASS    = 4,
	MAT_ORGANIC  = 5,
};

blockmaterial_t GetBlockMaterial(blocktype_t blockType);

struct BlockFeatures
{
	// Can Collide
	bool walkable;
	// Obscures faces
	bool solid;
	// Can select
	bool selectable;
	// Can break
	bool breakable;
};

BlockFeatures GetBlockFeatures(blocktype_t blockType);