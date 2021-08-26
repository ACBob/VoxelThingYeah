#include "world/chunk.hpp"
#include "world/block.hpp"

#include "utility/vector.hpp"

#ifdef CLIENTEXE
#include "rendering/shadermanager.hpp"
#endif

#include "world/block.hpp"
#include "physics.hpp"

#include "fastnoise.h"

#pragma once

#include <vector>

// #include "entities/entitybase.h"

class World {
	public:

#ifdef CLIENTEXE
		World(Shader *shader);
#elif SERVEREXE
		World();
#endif
		~World();

		Chunk *ChunkAtWorldPos(Vector pos);
		Chunk *ChunkAtChunkPos(Vector chunkPos);

		// Returns the block at the position in world coords
		// The given position is rounded by floor() before being used
		// If outside the world it returns a nullptr
		Block *BlockAtWorldPos(Vector pos);

		// Test against an infinitely small point centred on pos
		// Tests in world coordinates
		bool TestPointCollision(Vector pos);

		// Tests in world coordinates
		bool TestAABBCollision(AABB col);

		// Is the position within our place
		bool ValidChunkPos(const Vector pos);

		// Tick is the tick since the start of the game
		// FIXME: depending on if I got my calculation right, this will shit itself either in 1,000 or so years or 3.
		void WorldTick(int tick);
		
		struct PortableChunkRepresentation
		{
			uint32_t x,y,z;
			uint32_t blocks[CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z];
		};
		PortableChunkRepresentation GetWorldRepresentation(Vector pos);

		// Merges the PortableChunkRepresentation into us :)
		void UsePortable(PortableChunkRepresentation rep);

		std::vector<void*> ents;
		void AddEntity(void* e);

#ifdef CLIENTEXE
		void Render();

		// Shader we render with
		Shader *worldShader;
#endif

		std::vector<Chunk*> chunks;

		fnl_state noiseState;
};