#include "chunk.h"
#include "block.h"
#include "vector.h"
#include "shadermanager.h"

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