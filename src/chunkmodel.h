// TODO: genric model class

#include "vector.h"
#include "block.h"
#include <vector>

#pragma once

class ChunkModel {
	public:
		struct Vertex {
			Vertex(int a, int b, int c) {x=a;y=b;z=c;};
			float x, y, z;
			// TODO: UV, Texture, Colour, Lighting, Etc.
		};

		struct Face {
			Face(int a, int b, int c) {v=a;vv=b;vvv=c;};
			int v, vv, vvv;
		};
		std::vector<Vertex> vertices;
		std::vector<Face> faces;

		void Build(blocktype_t blocks[]);
};