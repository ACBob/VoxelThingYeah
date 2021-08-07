// TODO: genric model class

#include "utility/vector.h"
#include "world/block.h"
#include <vector>

#pragma once

class ChunkModel {
	public:
		ChunkModel(void* parent);

		struct Vertex {
			float x, y, z; // Position
			float nx, ny, nz; // Normal
			float u, v; // Texture U/V
			// TODO: Texture, Colour, Lighting, Etc.
		};

		struct Face {
			Face(int a = 0, int b = 0, int c = 0) {v=a;vv=b;vvv=c;};
			unsigned int v, vv, vvv;
		};
		std::vector<Vertex> vertices;
		std::vector<Face> faces;

		// Parent Chunk
		void *_chunk;

		void Build(Block blocks[], Vector pos);
};