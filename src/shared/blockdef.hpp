// Holds fancy block definition stuffy stuff
// TODO: more OOP way?

#include <types.hpp>

// size of X,Y,Z of blocks
#define BLOCKUNIT 1.0f

#pragma once

enum BLOCKID {
	BLCK_NONE = -1, // Generic error block, i.e it doesn't exist
	BLCK_AIR			 = 0,
	BLCK_STONE		 = 1,
	BLCK_DIRT		 = 2,
	BLCK_GRASS		 = 3,
	BLCK_COBBLE		 = 4,
	BLCK_PLANKS		 = 5,
	BLCK_BEDROCK		 = 6,
	BLCK_GLASS		 = 7,
	BLCK_LOG			 = 8,
	BLCK_LEAVES		 = 9,
	BLCK_WATER		 = 10,
	BLCK_WATERSRC	 = 11,
	BLCK_LAVA		 = 12,
	BLCK_LAVASRC		 = 13,
	BLCK_COALORE	 = 14,
	BLCK_IRONORE	 = 15,
	BLCK_FLOWER		 = 16,
	BLCK_SAND		 = 17,
	BLCK_SANDSTONE	 = 18,
	BLCK_BRICKS		 = 19,
	BLCK_TALLGRASS		 = 20,
	BLCK_MOSSYCOBBLE	 = 21,
	BLCK_SNOW		 = 22,
	BLCK_SNOWGRASS	 = 23,
	BLCK_ICE			 = 24,
	BLCK_WOOL		 = 25
};

enum MATERIAL {
	MAT_NONE	= 0, // Invalid
	MAT_STONE	= 1, // Stone, Rock
	MAT_LOOSE	= 2, // Dirt, Gravel
	MAT_WOOD	= 3, // It's wood
	MAT_GLASS	= 4, // It's glass
	MAT_ORGANIC = 5, // Grass, Leaves, Plants
	MAT_LIQUID	= 6, // Water, Lava
	MAT_DUST	= 7, // Sand
	MAT_FABRIC  = 8  // Wool
};

using BLOCKVAL = uint16_t;