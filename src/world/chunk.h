#include "utility/vector.h"

#include "world/block.h"

#include "rendering/chunkmodel.h"
#include "rendering/modelrenderer.h"
#include "rendering/shadermanager.h"

#define CHUNKSIZE_X 16
#define CHUNKSIZE_Y 16
#define CHUNKSIZE_Z 16

// Why yes, I am a C++ Programmer, how could you tell?
#define CHUNK3D_TO_1D(x, y, z) x + y*CHUNKSIZE_X + z*CHUNKSIZE_X*CHUNKSIZE_Z
#define CHUNK1D_TO_3D(i, x, y, z) z = round(i / (CHUNKSIZE_X * CHUNKSIZE_Y)); y = round((i - z * CHUNKSIZE_X * CHUNKSIZE_Y) / CHUNKSIZE_X); x = i - CHUNKSIZE_X * (y + CHUNKSIZE_Y * z)

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
		Chunk();
		~Chunk();
		
		void Generate();

		Chunk* Neighbour(Direction dir);

		ChunkPos worldPos;

		Block* GetBlockAtLocal(Vector pos);

		void RebuildMdl();

		void Render();

		void Update();

		Vector PosToWorld(Vector pos);

		// Flat array of blocks, access with
		// Indexed with [x + SIZEX * (y + SIZEZ * z)]
		Block blocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];

		ChunkModel mdl;

		// ChunkManager pointer (can't set type because circular include :lenny:)
		void* chunkMan = nullptr;
};

bool ValidChunkPosition(Vector pos);