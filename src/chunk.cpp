#include "chunk.h"

#include <random>

Chunk::Chunk() :
	rend(&mdl)
{
	// FOR NOW: start off filled with blocks
	for (int i = 0; i < sizeof(blocks) / sizeof(Block); i++)
	{
		blocks[i].blockType = blocktype_t(random() % 4);
	}

	RebuildMdl();
}

void Chunk::Render(shader_t shader)
{
	rend.Render(shader);
}

void Chunk::RebuildMdl()
{
	mdl.Build(blocks);
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