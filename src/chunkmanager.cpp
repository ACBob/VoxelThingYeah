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
				chunks.emplace_back(pos);
			}
		}
	}
	// DBUG
	// chunks.emplace_back(Vector(0,0,0));
	// chunks.emplace_back(Vector(1,0,0));
}

void ChunkManager::Render()
{
	for (int i = 0; i < chunks.size(); i++)
		chunks[i].Render();
}