#include "jeneration.hpp"

#include "world.hpp"

#include <math.h>

OverworldJeneration::OverworldJeneration()
{
	baseNoise = fnlCreateState();
	baseNoise.seed = seed;
	baseNoise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	baseNoise.frequency = 0.025;

	dirtNoise = fnlCreateState();
	dirtNoise.seed = seed + 230;
	dirtNoise.noise_type = FNL_NOISE_PERLIN;
	dirtNoise.frequency = 0.0125;

	oreNoise = fnlCreateState();
	oreNoise.seed = seed - 2587;
	oreNoise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	oreNoise.frequency = 0.075;
	oreNoise.octaves = 4;
	oreNoise.lacunarity = 0.6;
	oreNoise.gain = 1.3;
	oreNoise.weighted_strength = 0.8;
}

// Generates the base stone skeleton
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

		float noiseData = 1 + fnlGetNoise3D(&baseNoise, WorldPosition.x,WorldPosition.y,WorldPosition.z);
		float percentToTopSurface = 1.0f - (WorldPosition.y/32.0f);
		noiseData *= percentToTopSurface;


		c->blocks[i].blockType = noiseData > 0.7 ? blocktype_t::STONE : blocktype_t::AIR;
	}
}

// Replaces the surface and sometimes under that with block suiteable for the biome
void OverworldJeneration::BiomeBlocks(Chunk *c)
{
	// Grassification
	for (int x = 0; x < CHUNKSIZE_X; x++)
	{
		for (int z = 0; z < CHUNKSIZE_Z; z++)
		{
			int grassDepth = 2 + (2 * (1+fnlGetNoise2D(&dirtNoise, x+c->GetPosInWorld().x, z+c->GetPosInWorld().z)));

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

// Decorates with ores, plants, etc.
void OverworldJeneration::Decorate(Chunk *c)
{
	// Ore
	for (int i = 0; i < sizeof(c->blocks) / sizeof(Block); i++)
	{
		if (c->blocks[i].blockType != STONE)
			continue;

		int x, y, z;
		CHUNK1D_TO_3D(i, x, y, z);
		Vector WorldPosition = c->PosToWorld(Vector(x,y,z));

		float noiseData = fnlGetNoise3D(&oreNoise, WorldPosition.x,WorldPosition.y,WorldPosition.z) * 1.1;

		if (noiseData > 0.9)
			c->blocks[i].blockType = ORE_COAL;
	}
}

void OverworldJeneration::Generate(Chunk *c)
{
	GenBase(c);
	BiomeBlocks(c);
	Decorate(c);
}