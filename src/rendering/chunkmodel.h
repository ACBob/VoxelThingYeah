// TODO: genric model class

#include "utility/vector.h"
#include "world/block.h"
#include "model.h"
#include <vector>

#pragma once

class ChunkModel : public Model {
	public:
		ChunkModel(void* parent);

		// Parent Chunk
		void *_chunk;

		void Build(Block blocks[], Vector pos);
};