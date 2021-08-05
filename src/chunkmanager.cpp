#include "chunkmanager.h"

ChunkManager::ChunkManager()
{
	Vector pos;
	for (pos.x = 0; pos.x < 4; pos.x++)
	{
		for (pos.y = 0; pos.y < 4; pos.y++)
		{
			for (pos.z = 0; pos.z < 4; pos.z++)
			{
				Chunk chunk(pos);
				chunk.RebuildMdl();
				chunks.push_back(chunk);
			}
		}
	}
}

void ChunkManager::Render()
{
	for (int i = 0; i < chunks.size(); i++)
		chunks[i].Render();
}