#include "vector.h"
#include "block.h"
#include "chunkmodel.h"
#include "chunkrenderer.h"
#include "shadermanager.h"

#define CHUNKSIZE_X 16
#define CHUNKSIZE_Y 16
#define CHUNKSIZE_Z 16

#define CHUNK3D_TO_1D(x, y, z) x + y*CHUNKSIZE_X + z*CHUNKSIZE_X*CHUNKSIZE_Z

#pragma once

class Chunk
{
	public:
		Chunk();
		~Chunk();
		
		Vector WorldPos;

		Block GetBlockAtLocal(Vector pos);

		void RebuildMdl();

		void Render(shader_t shader);

		// Flat array of blocks, access with
		// Indexed with [x + SIZEX * (y + SIZEZ * z)]
		Block blocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];

		ChunkModel mdl;
		// Given the model to replace its' index buffers and such
		ChunkRenderer rend;
};

bool ValidChunkPosition(Vector pos);