#include "chunk.h"
#include "block.h"
#include "vector.h"
#include "shadermanager.h"

#include <vector>

class ChunkManager {
	public:

		ChunkManager();

		void Render();

		std::vector<Chunk> chunks;
};