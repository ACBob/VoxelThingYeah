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
				chunks.push_back(c);
			}
		}
	}

	// Now that all the chunks exist, generate and rebuild their models
	for (Chunk *c : chunks)
	{
		c->Generate();
		c->RebuildMdl();
	}
}
ChunkManager::~ChunkManager()
{
	// Destroy chunks
	for (Chunk *c : chunks)
		delete c;
}

// Return in good faith that it's a valid position
Chunk* ChunkManager::ChunkAtChunkPos(Vector pos)
{
	if (!ValidChunkPos(pos))
		return nullptr;

	for (Chunk* c : chunks)
		if (c->worldPos.pos == pos) return c;
}


// Return in good faith that it's a valid position
Chunk* ChunkManager::ChunkAtWorldPos(Vector pos)
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
	pos = pos.Floor();
	Chunk *chunk = ChunkAtWorldPos(pos);
	if (chunk == nullptr) return nullptr;
	Vector localPos = (pos - chunk->worldPos.ToWorld());

	return chunk->GetBlockAtLocal(localPos);
}

bool ChunkManager::ValidChunkPos(const Vector pos)
{
	for (Chunk* c : chunks)
		if (c->worldPos.pos == pos) return true;
	return false;
}

void ChunkManager::Render()
{
	for (Chunk* c : chunks)
	{
		c->Render();
	}
}