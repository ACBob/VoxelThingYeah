#include "chunkmanager.h"

ChunkManager::ChunkManager()
{
	chunks.reserve(64);

	Vector pos(0,0,0);
	for (pos.x = 0; pos.x < 2; pos.x++)
	{
		for (pos.y = 0; pos.y < 2; pos.y++)
		{
			for (pos.z = 0; pos.z < 2; pos.z++)
			{
				chunks.emplace_back(pos, this);
			}
		}
	}
	// DBUG
	// chunks.emplace_back(Vector(0,0,0));
	// chunks.emplace_back(Vector(1,0,0));
}

// Return in good faith that it's a valid position
Chunk* ChunkManager::ChunkAtChunkPos(Vector pos)
{
	if (!ValidChunkPos(pos))
		return nullptr;

	return &chunks[CHUNK3D_TO_1D(pos.x, pos.y, pos.z)];
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
	// TODO: negatives?
	int idx = CHUNK3D_TO_1D(pos.x, pos.y, pos.z);
	if (idx >= chunks.size() || idx < 0)
		return false;
	return true;
}

void ChunkManager::Render()
{
	for (int i = 0; i < chunks.size(); i++)
		chunks[i].Render();
}