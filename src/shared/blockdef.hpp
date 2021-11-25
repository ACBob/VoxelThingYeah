// Holds fancy block definition stuffy stuff
// Like ID
// For what used to be BlockFeatures, see BlockBase.cpp

#include <types.hpp>

// size of X,Y,Z of blocks
#define BLOCKUNIT 1.0f

#pragma once

// TODO: Rename to BLOCK_*
enum BLOCK : uint16_t {
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
	WOOL		 = 30,
	SOUNDMAKER	 = 31,
	STONEBRICKS  = 32,
	GRAVEL		 = 33,
	IRONBLOCK	 = 34,
	BOOKSHELF	 = 35,
	TNT 		 = 36,
	SNOWLAYER    = 37,
	SLAB		 = 38,
	GOLDBLOCK	 = 39,
	DIAMONDBLOCK = 40,
};
#define LASTBLOCK ( TNT + 1 )

// May be changed one day, so use this always
using BLOCKID  = uint16_t;
using BLOCKVAL = uint16_t;

enum BLOCKMATERIAL {
	MAT_NONE	= 0, // Invalid
	MAT_STONE	= 1, // Stone, Rock
	MAT_LOOSE	= 2, // Dirt, Gravel
	MAT_WOOD	= 3, // It's wood
	MAT_GLASS	= 4, // It's glass
	MAT_ORGANIC = 5, // Grass, Leaves, Plants
	MAT_LIQUID	= 6, // Water, Lava
	MAT_DUST	= 7, // Sand
	MAT_FABRIC	= 8, // Wool
	MAT_METAL   = 9, // Iron, Gold, etc
};

#ifdef CLIENTEXE
enum BLOCKMODEL { // The commented out ones are ideas for future models
	BLOCKMODEL_CUBE = 0, // Simple cube
	BLOCKMODEL_CROSS = 1, // A "X" - Like plants in Minecraft
	BLOCKMODEL_1LAYER = 2, // 1/16th of a block
	BLOCKMODEL_2LAYER = 3, // 2/16th of a block
	BLOCKMODEL_HALFBLOCK = 4, // 1/2 block
	// BLOCKMODEL_POUND = 3, // A "#" - Like farm plants in Minecraft
	// BLOCKMODEL_FLAME = 4, // Fire-like, every face looks like it's coming out of the block
};
#endif

const char *BlockMaterialSTR( BLOCKMATERIAL mat );

BLOCKMATERIAL GetBlockMaterial( BLOCKID id );