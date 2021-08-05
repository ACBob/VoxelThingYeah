#include "utility/vector.h"

#include "world/block.h"

#include "rendering/chunkmodel.h"
#include "rendering/chunkrenderer.h"
#include "rendering/shadermanager.h"

#define CHUNKSIZE_X 16
#define CHUNKSIZE_Y 16
#define CHUNKSIZE_Z 16

#define CHUNK3D_TO_1D(x, y, z) x + y*CHUNKSIZE_X + z*CHUNKSIZE_X*CHUNKSIZE_Z

#pragma once

class ChunkPos
{
	public:
		ChunkPos(Vector pos);

		// Converts it to block coordinates
		Vector ToWorld();

		Vector pos;
};

class Chunk
{
	public:
		Chunk(Vector pos, void* chunkMan);
		~Chunk();
		
		Chunk* Neighbour(Direction dir);

		ChunkPos worldPos;

		Block* GetBlockAtLocal(Vector pos);

		void RebuildMdl();

		void Render();

		// Flat array of blocks, access with
		// Indexed with [x + SIZEX * (y + SIZEZ * z)]
		Block blocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];

		ChunkModel mdl;
		// Given the model to replace its' index buffers and such
		ChunkRenderer rend;

		// ChunkManager pointer (can't set type because circular include :lenny:)
		void* chunkMan;
};

bool ValidChunkPosition(Vector pos);