#include "chunk.hpp"

#include <random>

#include "world.hpp"

#ifdef CLIENTEXE
Chunk::Chunk()
{
	for (int i = 0; i < CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z; i++)
		blocks[i].blockType = blocktype_t::AIR;
}
Chunk::~Chunk()
{
}
#elif SERVEREXE
Chunk::Chunk()
{
	for (int i = 0; i < CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z; i++)
		blocks[i].blockType = blocktype_t::AIR;
}
Chunk::~Chunk()
{
}
#endif

Chunk* Chunk::Neighbour(Direction dir)
{
	Vector neighbourPos = position + DirectionVector[dir];
	return (reinterpret_cast<World*>(chunkMan))->ChunkAtChunkPos(neighbourPos);
}

#ifdef CLIENTEXE
void Chunk::Render()
{
	mdl.Render();
}

void Chunk::RebuildMdl()
{
	BuildChunkModel(mdl, blocks, GetPosInWorld(), this);
}
#endif

// Takes a coordinate and returns a vector in world coordinates relative to this chunk
// Intended to be used by in-chunk coords but doesn't throw a hissyfit if it's not
Vector Chunk::PosToWorld(Vector pos)
{
	return GetPosInWorld() + pos;
}

void Chunk::Update()
{
#ifdef CLIENTEXE
	// Chunk update makes neighbours and ourself update our model
	// Hahahahahahah slow
	RebuildMdl();
	for (int i = 0; i < 6; i++)
	{
		Chunk *neighbour = Neighbour((Direction)i);
		if (neighbour != nullptr)
			neighbour->RebuildMdl();
	}
#endif

	outdated = true;
}

Block *Chunk::GetBlockAtLocal(Vector pos)
{
	if (!ValidChunkPosition(pos))
		return nullptr;
	return &blocks[int(CHUNK3D_TO_1D(pos.x, pos.y, pos.z))];
}

bool ValidChunkPosition(Vector pos)
{
	// If the position is valid
	if (
		pos.x < 0 || pos.y < 0 || pos.z < 0 ||
		pos.x >= CHUNKSIZE_X || pos.y >= CHUNKSIZE_Y || pos.z >= CHUNKSIZE_Z
	)
	{
		return false;
	}
	return true;
}