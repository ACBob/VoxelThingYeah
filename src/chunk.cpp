#include "chunk.h"

#include <random>


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

Chunk::Chunk(Vector pos) :
	rend(&mdl),
	worldPos(pos)
{
	// FOR NOW: start off filled with blocks
	for (int i = 0; i < sizeof(blocks) / sizeof(Block); i++)
	{
		blocks[i].blockType = blocktype_t(random() % 4);
	}

	RebuildMdl();
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

Chunk::~Chunk()
{

}

Block Chunk::GetBlockAtLocal(Vector pos)
{
	return blocks[int(CHUNK3D_TO_1D(pos.x, pos.y, pos.z))];
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