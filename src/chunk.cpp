#include "chunk.h"

#include <random>

Chunk::Chunk()
{
	// FOR NOW: start off filled with blocks
	for (int i = 0; i < sizeof(blocks); i++)
		blocks[i] = true;

	mdl.Build(blocks);
}

Chunk::~Chunk()
{

}

blocktype_t Chunk::GetBlockAtLocal(Vector pos)
{
	return blocks[int(pos.x * CHUNKSIZE_Y * CHUNKSIZE_Z + pos.y * CHUNKSIZE_Z + pos.z)];
}

bool ValidChunkPosition(Vector pos)
{
	// If the position is valid
	if (
		pos.x < 0 || pos.y < 0 || pos.z < 0 ||
		pos.x > CHUNKSIZE_X || pos.y > CHUNKSIZE_Y || pos.z > CHUNKSIZE_Z
	)
	{
		return false;
	}
	return true;
}