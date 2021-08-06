#include "world/chunk.h"
#include "world/block.h"

#include "utility/vector.h"

#include "rendering/shadermanager.h"

#pragma once

#include <vector>
#include <map>

class ChunkManager {
	public:

		ChunkManager();
		~ChunkManager();

		void Render();

		Chunk *ChunkAtBlockPos(Vector pos);
		Chunk *ChunkAtChunkPos(Vector chunkPos);

		// Is the position within our place
		bool ValidChunkPos(Vector pos);

		std::map<Vector, Chunk*> chunks;
};