// TODO: genric model class

#include "utility/vector.h"
#include "world/block.h"
#include <vector>

#pragma once

class ChunkModel {
	public:
		ChunkModel(void* parent);

		struct Vertex {
			Vertex(int a = 69, int b = 69, int c = 69, float u = 0, float v = 0) {x=a;y=b;z=c;};
			float x, y, z;
			float u, v;
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