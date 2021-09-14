// Holds fancy block definition stuffy stuff
// TODO: more OOP way?

// size of X,Y,Z of blocks
#define BLOCKUNIT 1.0f

#pragma once

enum blocktype_t {
	AIR		 = 0,
	STONE	 = 1,
	DIRT	 = 2,
	GRASS	 = 3,
	COBBLE	 = 4,
	PLANKS	 = 5,
	BEDROCK	 = 6,
	GLASS	 = 7,
	LOG		 = 8,
	LEAVES	 = 9,
	WATER	 = 10,
	LAVA	 = 11,
	ORE_COAL = 12,
	ORE_IRON = 13,
	FLOWER	 = 14,
};

enum blockmaterial_t {
	MAT_NONE	= 0, // Invalid
	MAT_STONE	= 1, // Stone, Rock
	MAT_LOOSE	= 2, // Dirt, Gravel
	MAT_WOOD	= 3, // It's wood
	MAT_GLASS	= 4, // It's glass
	MAT_ORGANIC = 5, // Grass, Leaves, Plants
	MAT_LIQUID  = 6, // Water, Lava
};

enum blockobscurerule_t {
	OBSCURERULE_ALWAYS	= 0, // If we can, we do
	OBSCURERULE_SIMILAR = 1, // Only do it if it's the same as us
	OBSCURERULE_NEVER	= 2	 // Never obscure faces
};

enum blockmodel_t {
	BLOCKMODEL_CUBE	 = 0, // Default cube
	BLOCKMODEL_PLANT = 1, // X-Shape
};

blockmaterial_t GetBlockMaterial( blocktype_t blockType );

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
	// IS a liquid
	bool isLiquid;
	// Every THIS tick, flow
	int liquidSpeed;
	// Model to use
	blockmodel_t model;
	// Obscure Rule
	blockobscurerule_t rule;
};

BlockFeatures GetBlockFeatures( blocktype_t blockType );