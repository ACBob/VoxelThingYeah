// Holds fancy block definition stuffy stuff
// TODO: more OOP way?

#include <types.hpp>

// size of X,Y,Z of blocks
#define BLOCKUNIT 1.0f

#pragma once

enum blocktype_t {
	AIR			 = 0,
	STONE		 = 1,
	DIRT		 = 2,
	GRASS		 = 3,
	COBBLE		 = 4,
	PLANKS		 = 5,
	BEDROCK		 = 6,
	GLASS		 = 7,
	LOG			 = 8,
	LEAVES		 = 9,
	WATER		 = 10,
	WATERSRC	 = 11,
	LAVA		 = 12,
	LAVASRC		 = 13,
	ORE_COAL	 = 14,
	ORE_IRON	 = 15,
	FLOWER		 = 16,
	SAND		 = 17,
	SANDSTONE	 = 18,
	BRICKS		 = 19,
	LGRASS		 = 20,
	MOSSCBBLE	 = 21,
	SNOW		 = 22,
	SNOWGRASS	 = 23,
	ICE			 = 24,
	LIGHT_YELLOW = 25,
	LIGHT_WHITE	 = 26,
	LIGHT_RED	 = 27,
	LIGHT_GREEN	 = 28,
	LIGHT_BLUE	 = 29,
	WOOL		 = 30
};

enum blockmaterial_t {
	MAT_NONE	= 0, // Invalid
	MAT_STONE	= 1, // Stone, Rock
	MAT_LOOSE	= 2, // Dirt, Gravel
	MAT_WOOD	= 3, // It's wood
	MAT_GLASS	= 4, // It's glass
	MAT_ORGANIC = 5, // Grass, Leaves, Plants
	MAT_LIQUID	= 6, // Water, Lava
	MAT_DUST	= 7,	 // Sand
	MAT_FABRIC  = 8
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

const char *BlockMaterialSTR( blockmaterial_t mat );

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
	// IS a source of a liquid (i.e act infinite and create flow)
	bool isLiquidSource;
	// is a light source
	bool isLightSource;
	// Opaqueness, similar to colour, set to 0xFFF to act fully opaque
	uint16_t opaqueness;
	// The colour of light
	uint16_t lightColour;
	// Flow/Source variants
	blocktype_t liquidFlow;
	blocktype_t liquidSource;
	// How far the liquid should flow
	uint8_t liquidRange;
	// Every THIS tick, flow
	int liquidSpeed;
	// Model to use
	blockmodel_t model;
	// Obscure Rule
	blockobscurerule_t rule;
};

BlockFeatures GetBlockFeatures( blocktype_t blockType );