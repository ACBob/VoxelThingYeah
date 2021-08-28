// Holds fancy block definition stuffy stuff
// TODO: more OOP way?

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
	WATER    = 10,
};

enum blockmaterial_t {
	MAT_NONE     = 0, // Invalid
	MAT_STONE    = 1, // Stone, Rock
	MAT_LOOSE    = 2, // Dirt, Gravel
	MAT_WOOD     = 3, // It's wood
	MAT_GLASS    = 4, // It's glass
	MAT_ORGANIC  = 5, // Grass, Leaves, Plants
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
	// Can replace with a liquid
	bool floodable;
};

BlockFeatures GetBlockFeatures(blocktype_t blockType);