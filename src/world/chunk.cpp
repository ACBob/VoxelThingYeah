#include "chunk.h"

#include <random>

#include "chunkmanager.h"


ChunkPos::ChunkPos(Vector pos)
{
	this->pos = pos;
}

Vector ChunkPos::ToWorld()
{
	Vector g = Vector(pos);
	g.x *= CHUNKSIZE_X;
	g.y *= CHUNKSIZE_Y;
	g.z *= CHUNKSIZE_Z;
	return g;
}

Chunk::Chunk() :
	mdl(this),
	rend(&mdl),
	worldPos(Vector())
{
}

void Chunk::Generate()
{
	// FOR NOW: start off filled with blocks
	for (int i = 0; i < sizeof(blocks) / sizeof(Block); i++)
	{
		// blocks[i].blockType = blocktype_t(random() % 4);
		int x, y, z;
		CHUNK1D_TO_3D(i, x, y, z);
		Vector Worldposition = PosToWorld(Vector(x,y,z));

		if (Worldposition.y == 8)
			blocks[i].blockType = blocktype_t::GRASS;
		else if (Worldposition.y < 8)
			blocks[i].blockType = random() % 2 > 0 ? blocktype_t::COBBLE : blocktype_t::AIR;
		else
			blocks[i].blockType = blocktype_t::AIR;
	}
}

Chunk* Chunk::Neighbour(Direction dir)
{
	Vector neighbourPos = worldPos.pos + DirectionVector[dir];
	return (reinterpret_cast<ChunkManager*>(chunkMan))->ChunkAtChunkPos(neighbourPos);
}

void Chunk::Render()
{
	rend.Render();
}

void Chunk::RebuildMdl()
{
	mdl.Build(blocks, worldPos.ToWorld());
	rend.Populate();
}

// Takes a coordinate and returns a vector in world coordinates relative to this chunk
// Intended to be used by in-chunk coords but doesn't throw a hissyfit if it's not
Vector Chunk::PosToWorld(Vector pos)
{
	return worldPos.ToWorld() + pos;
}

Chunk::~Chunk()
{
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