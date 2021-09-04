#include "jeneration.hpp"

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
		Vector Worldposition = c->PosToWorld(Vector(x,y,z));

		// Make the block aware of our existence
		c->blocks[i].chunk = this;

		float noiseData = 1 + fnlGetNoise3D(&noise, Worldposition.x,Worldposition.y,Worldposition.z);
		float percentToTopWorld = 1 - ((32 + Worldposition.y) / 64.0f);
		noiseData *= (percentToTopWorld * 1.0f);


		if (Worldposition.y == 8)
			c->blocks[i].blockType = noiseData > 0.2 ? blocktype_t::GRASS : blocktype_t::AIR;
		else if (Worldposition.y == -32)
			c->blocks[i].blockType = blocktype_t::BEDROCK;
		else if (Worldposition.y < 6)
			c->blocks[i].blockType = noiseData > 0.2 ? blocktype_t::STONE : blocktype_t::AIR;
		else if (Worldposition.y < 8)
			c->blocks[i].blockType = noiseData > 0.2 ? blocktype_t::DIRT : blocktype_t::AIR;
		else
			c->blocks[i].blockType = blocktype_t::AIR;
	}
}