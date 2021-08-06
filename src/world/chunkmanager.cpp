#include "chunkmanager.h"

ChunkManager::ChunkManager()
{
	chunks = {};

	Vector pos(0,0,0);
	for (pos.x = 0; pos.x < 2; pos.x++)
	{
		for (pos.y = 0; pos.y < 2; pos.y++)
		{
			for (pos.z = 0; pos.z < 2; pos.z++)
			{
				Chunk *c = new Chunk();
				c->worldPos = pos;
				c->chunkMan = this;
				c->RebuildMdl();

				chunks[Vector(pos)] = c;
			}
		}
	}
	// DBUG
	// chunks.emplace_back(Vector(0,0,0));
	// chunks.emplace_back(Vector(1,0,0));
}
ChunkManager::~ChunkManager()
{
	// Destroy chunks
	for (auto &c : chunks)
		delete c.second;
}

// Return in good faith that it's a valid position
Chunk* ChunkManager::ChunkAtChunkPos(Vector pos)
{
	if (!ValidChunkPos(pos))
		return nullptr;

	return chunks.at(pos);
}


// Return in good faith that it's a valid position
Chunk* ChunkManager::ChunkAtBlockPos(Vector pos)
{
	pos.x /= 16.0f;
	pos.y /= 16.0f;
	pos.z /= 16.0f;

	pos.x = ceil(pos.x);
	pos.y = ceil(pos.y);
	pos.z = ceil(pos.z);

	return ChunkAtChunkPos(pos);
}

bool ChunkManager::ValidChunkPos(Vector pos)
{
	if (chunks.count(pos))
		return true;
	return false;
}

void ChunkManager::Render()
{
	for (auto &chunk : chunks)
	{
		chunk.second->Render();
	}
}