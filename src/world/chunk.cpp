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

		// Make the block aware of our existence
		blocks[i].chunk = this;

		if (Worldposition.y == 8)
			blocks[i].blockType = blocktype_t::GRASS;
		else if (Worldposition.y == -32)
			blocks[i].blockType = blocktype_t::BEDROCK;
		else if (Worldposition.y < 6)
			blocks[i].blockType = blocktype_t::STONE;
		else if (Worldposition.y < 8)
			blocks[i].blockType = blocktype_t::DIRT;
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
	mdl.Render();
}

void Chunk::RebuildMdl()
{
	mdl.Build(blocks, worldPos.ToWorld());
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

void Chunk::Update()
{
	// Chunk update makes neighbours and ourself update our model
	// Hahahahahahah slow
	RebuildMdl();
	for (int i = 0; i < 6; i++)
	{
		Chunk *neighbour = Neighbour((Direction)i);
		if (neighbour != nullptr)
			neighbour->RebuildMdl();
	}
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