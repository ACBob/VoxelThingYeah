#include "chunk.hpp"

#include <random>

#include "world.hpp"

#ifdef CLIENTEXE
Chunk::Chunk()
{
}
Chunk::~Chunk()
{
}
#elif SERVEREXE
Chunk::Chunk()
{
}
Chunk::~Chunk()
{
}
#endif

void Chunk::Generate(fnl_state noise)
{
	// FOR NOW: start off filled with blocks
	for (int i = 0; i < sizeof(blocks) / sizeof(Block); i++)
	{
		// blocks[i].blockType = blocktype_t(random() % 4);
		int x, y, z;
		CHUNK1D_TO_3D(i, x, y, z);
		Vector Worldposition = PosToWorld(Vector(x,y,z));

		// Make the block aware of our existence
		blocks[i].chunk = this;

#ifdef SERVEREXE
		float noiseData = 1 + fnlGetNoise3D(&noise, Worldposition.x,Worldposition.y,Worldposition.z);
		float percentToTopWorld = 1 - ((32 + Worldposition.y) / 64.0f);
		noiseData *= (percentToTopWorld * 1.0f);


		if (Worldposition.y == 8)
			blocks[i].blockType = noiseData > 0.2 ? blocktype_t::GRASS : blocktype_t::AIR;
		else if (Worldposition.y == -32)
			blocks[i].blockType = blocktype_t::BEDROCK;
		else if (Worldposition.y < 6)
			blocks[i].blockType = noiseData > 0.2 ? blocktype_t::STONE : blocktype_t::AIR;
		else if (Worldposition.y < 8)
			blocks[i].blockType = noiseData > 0.2 ? blocktype_t::DIRT : blocktype_t::AIR;
		else
			blocks[i].blockType = blocktype_t::AIR;
#elif CLIENTEXE
		blocks[i].blockType = blocktype_t::AIR;
#endif
	}
}

Chunk* Chunk::Neighbour(Direction dir)
{
	Vector neighbourPos = position + DirectionVector[dir];
	return (reinterpret_cast<World*>(chunkMan))->ChunkAtChunkPos(neighbourPos);
}

#ifdef CLIENTEXE
void Chunk::Render()
{
	mdl.Render();
}

void Chunk::RebuildMdl()
{
	BuildChunkModel(mdl, blocks, GetPosInWorld(), this);
}
#endif

// Takes a coordinate and returns a vector in world coordinates relative to this chunk
// Intended to be used by in-chunk coords but doesn't throw a hissyfit if it's not
Vector Chunk::PosToWorld(Vector pos)
{
	return GetPosInWorld() + pos;
}

void Chunk::Update()
{
#ifdef CLIENTEXE
	// Chunk update makes neighbours and ourself update our model
	// Hahahahahahah slow
	RebuildMdl();
	for (int i = 0; i < 6; i++)
	{
		Chunk *neighbour = Neighbour((Direction)i);
		if (neighbour != nullptr)
			neighbour->RebuildMdl();
	}
#endif

	outdated = true;
}

Block *Chunk::GetBlockAtLocal(Vector pos)
{
	if (!ValidChunkPosition(pos))
		return nullptr;
	return &blocks[int(CHUNK3D_TO_1D(pos.x, pos.y, pos.z))];
}

bool ValidChunkPosition(Vector pos)
{
	// If the position is valid
	if (
		pos.x < 0 || pos.y < 0 || pos.z < 0 ||
		pos.x >= CHUNKSIZE_X || pos.y >= CHUNKSIZE_Y || pos.z >= CHUNKSIZE_Z
	)
	{
		return false;
	}
	return true;
}