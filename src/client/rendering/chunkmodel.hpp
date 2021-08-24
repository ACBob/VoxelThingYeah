// TODO: genric model class

#include "utility/vector.hpp"
#include "world/block.hpp"
#include "model.hpp"
#include <vector>

#pragma once

class ChunkModel : public Model {
	public:
		ChunkModel(void* parent);

		// Parent Chunk
		void *_chunk;

		void Build(Block blocks[], Vector pos);
};