#include "chunk.h"
#include "block.h"
#include "vector.h"
#include "shadermanager.h"

#include <vector>

class ChunkManager {
	public:

		ChunkManager();

		// TODO: Internal shader storage / our own shader class
		void Render(shader_t shader);

		std::vector<Chunk> chunks;
};