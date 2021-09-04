#include "world.hpp"

#include "entities/entitybase.hpp"

#include "physics.hpp"

#include "shared/seethe.h"

#ifdef SERVEREXE
#include "server/cvar_serverside.hpp"
#endif

#ifdef CLIENTEXE
World::World(Shader *shader, Shader *entShader) :
	worldShader(shader),
	entityShader(entShader)
#elif SERVEREXE
World::World()
#endif
{
	chunks = {};

	noiseState = fnlCreateState();
	noiseState.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noiseState.frequency = 0.025;

	// Now that all the chunks exist, generate and rebuild their models
	for (Chunk *c : chunks)
	{
		c->Generate(noiseState);
	}
#ifdef CLIENTEXE
	for (Chunk *c : chunks)
	{
		c->RebuildMdl();
	}
#endif
}
World::~World()
{
	// Destroy chunks
	for (Chunk *c : chunks)
		delete c;
}

// Return in good faith that it's a valid position
Chunk* World::ChunkAtChunkPos(Vector pos)
{
	if (!ValidChunkPos(pos))
		return nullptr;

	for (Chunk* c : chunks)
		if (c->position == pos) return c;
}

// Tries to get a chunk and generates a new one if it can't find one
Chunk* World::GetChunkGenerateAtWorldPos(Vector pos)
{
	Chunk *c = ChunkAtWorldPos(pos);
	if (c != nullptr)
		return c;
	
	c = new Chunk();
	c->position = (pos / Vector(CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z)).Floor();
	c->chunkMan = this;
#ifdef CLIENTEXE
	c->mdl.position = c->GetPosInWorld();
	c->mdl.SetShader(worldShader);
#endif
	c->Generate(noiseState);

	chunks.push_back(c);

	return c;
}

// Return in good faith that it's a valid position
Chunk* World::ChunkAtWorldPos(Vector pos)
{
	pos = pos / Vector(CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z);
	pos = pos.Floor();

	return ChunkAtChunkPos(pos);
}

Block *World::BlockAtWorldPos(Vector pos)
{
	pos = pos.Floor();
	Chunk *chunk = ChunkAtWorldPos(pos);
	if (chunk == nullptr) return nullptr;
	Vector localPos = (pos - chunk->GetPosInWorld());

	return chunk->GetBlockAtLocal(localPos);
}

bool World::ValidChunkPos(const Vector pos)
{
	for (Chunk* c : chunks)
		if (c->position == pos) return true;
	return false;
}

void World::AddEntity(void *e)
{
	ents.push_back(e);
	((EntityBase*)e)->Spawn();
#ifdef CLIENTEXE
	((EntityBase*)e)->SetShader(entityShader);
#endif
}

void* World::GetEntityByName(const char *name)
{
	for (void *e : ents)
	{
		EntityBase *ent = reinterpret_cast<EntityBase*>(e);
		if (ent->name == name)
			return e;
	}
	return nullptr;
}

#ifdef CLIENTEXE
void World::Render()
{
	for (Chunk* c : chunks)
	{
		c->Render();
	}
	for (void *ent : ents)
	{
		((EntityBase*)ent)->Render();
	}
}
#endif

bool World::TestPointCollision(Vector pos)
{
	Block *b = BlockAtWorldPos(pos);
	if (b == nullptr)
		return false;
	BlockFeatures bF = GetBlockFeatures(b->blockType);
	if (!bF.walkable) 
		return false;
	
	pos = pos - pos.Floor();
	return AABB(pos.Floor(), Vector(1,1,1), Vector(0)).TestPointCollide(pos);
}

bool World::TestAABBCollision(AABB col)
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
		BlockFeatures bF = GetBlockFeatures(blockType);
		if (!bF.walkable) 
			continue;

		if (
			col.TestCollide(
				AABB(chunk->GetPosInWorld() + Vector(x,y,z), Vector(1,1,1), Vector(0))
			)
		)
			return true;
	}

	return false;
}

void World::WorldTick(int tickN)
{
	for (int i = 0; i < ents.size(); i++)
	{
		void* ent = ents[i];

		if (reinterpret_cast<EntityBase*>(ent)->isKilled)
		{
			// Clear from world
			con_debug("removing entity");
			ents.erase(ents.begin()+i);

			continue;
		}
		reinterpret_cast<EntityBase*>(ent)->Tick();
#ifdef SERVEREXE
		reinterpret_cast<EntityBase*>(ent)->PhysicsTick(sv_tickms->GetInt() / 1000.0f, this);
#else
		// TODO: get tick speed from server
		reinterpret_cast<EntityBase*>(ent)->PhysicsTick(0.05, this);
#endif
	}

#ifdef SERVEREXE
	for (Chunk* chunk : chunks)
	{
		// Generally we should avoid rebuilding the universe every tick
		bool rebuild = false;
		std::vector<Vector> liquidBlocks = {};
		for (int i = 0; i < (CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z); i ++)
		{
			int x,y,z;
			CHUNK1D_TO_3D(i, x,y,z);

			blocktype_t blockType = chunk->blocks[i].blockType;

			// Every fifth tick
			// TODO: Search for all water THEN put it into a list easy to handle later (so we don't progressively fill existing water)
			// TODO: maybe liquid type? could be slow...
			BlockFeatures bF = GetBlockFeatures(blockType);
			if (tickN % 5 == 0 && bF.isLiquid)
			{
				liquidBlocks.push_back(Vector(x,y,z));
			}
		}

		for (Vector pos : liquidBlocks)
		{
			blocktype_t blockType = chunk->GetBlockAtLocal(pos)->blockType;
			for (int i = 0; i < 5; i++)
			{
				if (i == UP) i = DOWN;

				Vector dir = DirectionVector[i];
				Block *b = chunk->GetBlockAtLocal(Vector(pos.x,pos.y,pos.z)+dir);
				if (b == nullptr)
				{
					// It's not in *this* chunk
					Chunk *oChunk = chunk->Neighbour((Direction)i);
					if (oChunk == nullptr)
						continue; // Ok yeah it's outside reality
					Vector p = Vector(pos.x + dir.x,pos.y + dir.y,pos.z + dir.z) + (dir * -16);
					b = oChunk->GetBlockAtLocal(p);
					if (b == nullptr) 
						continue; //uh oh
				}
				BlockFeatures bF = GetBlockFeatures(b->blockType);
				if (bF.floodable)
				{
					b->blockType = blockType;
					rebuild = true;
				}
			}
		}

		if (rebuild)
			chunk->Update();
	}

	// Progress time
	timeOfDay ++;
#endif
	if (timeOfDay > 24000)
	{
		timeOfDay = 0;
	}
}

World::PortableChunkRepresentation World::GetWorldRepresentation(Vector pos)
{
	PortableChunkRepresentation crep;

	// if (!ValidChunkPos(pos))
	// {
	// 	con_critical("We've just returned garbage data for the portable chunk!");
	// 	return crep;
	// }

	Chunk *c = GetChunkGenerateAtWorldPos(pos * Vector(CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z));
	crep.x = c->position.x;
	crep.y = c->position.y;
	crep.z = c->position.z;

	for (int j = 0; j < CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z; j++)
	{
		crep.blocks[j] = c->blocks[j].blockType;
	}

	return crep;
}

void World::UsePortable(PortableChunkRepresentation rep)
{
	Chunk *c = GetChunkGenerateAtWorldPos(Vector(rep.x * CHUNKSIZE_X, rep.y * CHUNKSIZE_Y, rep.z * CHUNKSIZE_Z));
	if (c == nullptr)
	{
		con_error("WEE WOO WEE WOO");
		return;
	}

	for (int j = 0; j < CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z; j++)
	{
		c->blocks[j].blockType = (blocktype_t)rep.blocks[j];
	}
	
	c->Update();
}