#include "chunkmanager.h"

ChunkManager::ChunkManager()
{
	Vector pos(0,0,0);
	for (pos.x = 0; pos.x < 2; pos.x++)
	{
		for (pos.y = 0; pos.y < 2; pos.y++)
		{
			for (pos.z = 0; pos.z < 2; pos.z++)
			{
				Chunk* c = &chunks[pos];
				c->worldPos = pos;
				c->chunkMan = this;
				c->RebuildMdl();
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

	return &chunks.at(pos);
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
	int j = 0;
	std::map<const Vector, Chunk>::iterator i = chunks.begin();
	while (i != chunks.end())
	{
		i->second.Render();
		i++;
		j++;
	}
	j;
}