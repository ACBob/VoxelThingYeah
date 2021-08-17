#include "world/chunk.h"
#include "world/block.h"

#include <bitsery/bitsery.h>

#include "utility/vector.h"

#ifdef CLIENTEXE
#include "rendering/shadermanager.h"
#endif

#include "world/block.h"
#include "physics.h"

#include "fastnoise.h"

#pragma once

#include <vector>

// World coords are - to + these values
#define MAXCOORD_X 65535
#define MAXCOORD_Y 65535
#define MAXCOORD_Z 65535

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

			template <typename S>
			void serialize(S& s)
			{
				s.value4b(x);
				s.value4b(y);
				s.value4b(z);
				s.container4b(blocks);
			};
		};
		PortableChunkRepresentation GetWorldRepresentation(Vector pos);

		// Merges the PortableChunkRepresentation into us :)
		void UsePortable(PortableChunkRepresentation rep);

#ifdef CLIENTEXE
		void Render();

		// Shader we render with
		Shader *worldShader;
#endif

		std::vector<Chunk*> chunks;

		fnl_state noiseState;
};