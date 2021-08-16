#include "utility/vector.h"

#include "world/block.h"

#ifdef CLIENTEXE
#include "rendering/chunkmodel.h"
#include "rendering/modelrenderer.h"
#include "rendering/shadermanager.h"
#endif

#include "fastnoise.h"

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
		
		void Generate(fnl_state noise);

		Chunk* Neighbour(Direction dir);

		ChunkPos worldPos;

		Block* GetBlockAtLocal(Vector pos);

#ifdef CLIENTEXE
		void RebuildMdl();

		void Render();
#endif
		void Update();

		Vector PosToWorld(Vector pos);

		// Flat array of blocks, access with
		// Indexed with [x + SIZEX * (y + SIZEZ * z)]
		Block blocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];

#ifdef CLIENTEXE
		ChunkModel mdl;
#endif

		// World pointer (can't set type because circular include :lenny:)
		void* chunkMan = nullptr;
};

bool ValidChunkPosition(Vector pos);