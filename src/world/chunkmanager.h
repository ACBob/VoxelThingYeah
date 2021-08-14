#include "world/chunk.h"
#include "world/block.h"

#include "utility/vector.h"

#include "rendering/shadermanager.h"

#include "world/block.h"
#include "physics.h"

#include "fastnoise.h"

#pragma once

#include <vector>

// World coords are - to + these values
#define MAXCOORD_X 65535
#define MAXCOORD_Y 65535
#define MAXCOORD_Z 65535

class ChunkManager {
	public:

		ChunkManager(Shader *shader);
		~ChunkManager();

		void Render();

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

		// Tick is (for now) The MS Since the start of the game
		void WorldTick(int tickInMS);

		// Shader we render with
		Shader *worldShader;

		std::vector<Chunk*> chunks;

		fnl_state noiseState;
};