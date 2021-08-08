#include "world/chunk.h"
#include "world/block.h"

#include "utility/vector.h"

#include "rendering/shadermanager.h"

#include "world/block.h"

#pragma once

#include <vector>

class ChunkManager {
	public:

		ChunkManager();
		~ChunkManager();

		void Render();

		Chunk *ChunkAtWorldPos(Vector pos);
		Chunk *ChunkAtChunkPos(Vector chunkPos);

		// Returns the block at the position in world coords
		// The given position is rounded by ceil() before being used
		// If outside the world it returns a nullptr
		Block *BlockAtWorldPos(Vector pos);

		// Is the position within our place
		bool ValidChunkPos(const Vector pos);

		std::vector<Chunk*> chunks;
};