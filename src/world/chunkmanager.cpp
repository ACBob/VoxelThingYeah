#include "chunkmanager.h"

#include "physics.h"

ChunkManager::ChunkManager(Shader *shader) :
	worldShader(shader)
{
	chunks = {};

	noiseState = fnlCreateState();
	noiseState.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noiseState.frequency = 0.025;

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
				c->mdl.pos = c->worldPos.ToWorld();
				c->mdl.SetShader(worldShader);
				chunks.push_back(c);
			}
		}
	}

	// Now that all the chunks exist, generate and rebuild their models
	for (Chunk *c : chunks)
	{
		c->Generate(noiseState);
	}
	for (Chunk *c : chunks)
	{
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
	pos = pos / Vector(CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z);
	pos = pos.Floor();

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

bool ChunkManager::TestPointCollision(Vector pos)
{
	Block *b = BlockAtWorldPos(pos);
	if (b == nullptr)
		return false;
	pos = pos - pos.Floor();
	return b->blockType != blocktype_t::AIR && AABB(pos.Floor(), Vector(1,1,1), Vector(0)).TestPointCollide(pos);
}

bool ChunkManager::TestAABBCollision(AABB col)
{
	Chunk *chunk = ChunkAtWorldPos(col.pos);
	if (chunk == nullptr) return false;


	// Test A (pos)
	for (int i = 0; i < (CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z); i ++)
	{
		int x,y,z;
		CHUNK1D_TO_3D(i, x,y,z);
		
		// Don't collide with air
		blocktype_t blockType = chunk->blocks[i].blockType;
		if (blockType == blocktype_t::AIR) 
			continue;

		if (
			col.TestCollide(
				AABB(chunk->worldPos.ToWorld() + Vector(x,y,z), Vector(1,1,1), Vector(0))
			)
		)
			return true;
	}

	return false;
}