#include "world/chunk.h"
#include "world/block.h"

#include "utility/vector.h"

#include "rendering/shadermanager.h"

#pragma once

#include <vector>

class ChunkManager {
	public:

		ChunkManager();

		void Render();

		Chunk *ChunkAtBlockPos(Vector pos);
		Chunk *ChunkAtChunkPos(Vector chunkPos);

		// Is the position within our place
		bool ValidChunkPos(Vector pos);

		std::vector<Chunk> chunks;
};