#include "jeneration.hpp"

#include "world.hpp"

#include <math.h>

OverworldJeneration::OverworldJeneration()
{
	noise = fnlCreateState();
	noise.seed = seed;
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noise.frequency = 0.025;
}

void OverworldJeneration::Generate(Chunk *c)
{
	for (int i = 0; i < sizeof(c->blocks) / sizeof(Block); i++)
	{
		// blocks[i].blockType = blocktype_t(random() % 4);
		int x, y, z;
		CHUNK1D_TO_3D(i, x, y, z);
		Vector WorldPosition = c->PosToWorld(Vector(x,y,z));

		// Make the block aware of our existence
		c->blocks[i].chunk = c;

		float noiseData = 1 + fnlGetNoise3D(&noise, WorldPosition.x,WorldPosition.y,WorldPosition.z);
		float percentToTopWorld = 1.0f - (WorldPosition.y/32.0f);
		noiseData *= percentToTopWorld;


		c->blocks[i].blockType = noiseData > 0.7 ? blocktype_t::STONE : blocktype_t::AIR;
	}

	// Grassification
	for (int x = 0; x < CHUNKSIZE_X; x++)
	{
		for (int z = 0; z < CHUNKSIZE_Z; z++)
		{
			for (int y = CHUNKSIZE_Y; y > 0; y--)
			{
				int i = CHUNK3D_TO_1D(x,y,z);
				if (c->blocks[i].blockType == AIR)
					continue;
				
				Block *b = c->GetBlockAtLocal(Vector(x,y+1,z));
				if (b == nullptr) continue;

				if (b->blockType == AIR)
					c->blocks[i].blockType = GRASS;
				else if (b->blockType == GRASS)
					c->blocks[i].blockType = DIRT;
				
			}
		}
	}
}