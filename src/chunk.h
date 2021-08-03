#include "vector.h"
#include "block.h"
#include "chunkmodel.h"

#define CHUNKSIZE_X 2
#define CHUNKSIZE_Y 2
#define CHUNKSIZE_Z 2

#pragma once

class Chunk
{
	public:
		Chunk();
		~Chunk();
		
		Vector WorldPos;

		blocktype_t GetBlockAtLocal(Vector pos);

		// Flat array of blocks, access with
		// Indexed with [x + SIZEX * (y + SIZEZ * z)]
		blocktype_t blocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];

		ChunkModel mdl;
};