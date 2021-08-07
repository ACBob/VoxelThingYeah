#include "chunkmanager.h"

ChunkManager::ChunkManager()
{
	chunks = {};

	Vector pos(0,0,0);
	for (pos.x = -2; pos.x < 2; pos.x++)
	{
		for (pos.y = -2; pos.y < 2; pos.y++)
		{
			for (pos.z = -2; pos.z < 2; pos.z++)
			{
				Chunk *c = new Chunk();
				c->worldPos = pos;
				c->chunkMan = this;
				chunks.insert(std::make_pair(pos, c));
			}
		}
	}

	// Now that all the chunks exist, generate and rebuild their models
	for (auto c : chunks)
	{
		c.second->Generate();
		c.second->RebuildMdl();
	}
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
	pos.x /= float(CHUNKSIZE_X);
	pos.y /= float(CHUNKSIZE_Y);
	pos.z /= float(CHUNKSIZE_Z);

	pos.x = ceil(pos.x);
	pos.y = ceil(pos.y);
	pos.z = ceil(pos.z);

	return ChunkAtChunkPos(pos);
}

Block *ChunkManager::BlockAtWorldPos(Vector pos)
{
	Chunk *chunk = ChunkAtBlockPos(pos);
	if (chunk == nullptr) return nullptr;
	pos = pos - chunk->worldPos.ToWorld();
	return chunk->GetBlockAtLocal(pos);
}

bool ChunkManager::ValidChunkPos(const Vector pos)
{
	if (chunks.count(pos) > 0)
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