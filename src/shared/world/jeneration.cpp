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

void OverworldJeneration::GenBase(Chunk *c)
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
		float percentToTopSurface = 1.0f - (WorldPosition.y/32.0f);
		noiseData *= percentToTopSurface;


		c->blocks[i].blockType = noiseData > 0.7 ? blocktype_t::STONE : blocktype_t::AIR;
	}
}

void OverworldJeneration::BiomeBlocks(Chunk *c)
{
	// Grassification
	for (int x = 0; x < CHUNKSIZE_X; x++)
	{
		for (int z = 0; z < CHUNKSIZE_Z; z++)
		{

			int grassDepth = 3;

			// We follow down to -1 so we can alter the blocks in the chunk below
			for (int y = CHUNKSIZE_Y; y > -1; y--)
			{
				Block *blk = c->GetBlockAtLocal(Vector(x,y,z));
				if (blk != nullptr && blk->blockType == AIR)
					continue;
				
				Block *b = c->GetBlockAtLocal(Vector(x,y+1,z));
				if (b == nullptr)
					continue;

				if (b->blockType == AIR)
				{
					blk->blockType = GRASS;
					grassDepth --;
				}
				else if (b->blockType == GRASS || b->blockType == DIRT && grassDepth > 0)
				{
					blk->blockType = DIRT;
					grassDepth --;
				}
				
			}
		}
	}
}

void OverworldJeneration::Generate(Chunk *c)
{
	GenBase(c);
	BiomeBlocks(c);
}