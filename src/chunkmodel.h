// TODO: genric model class

#include "vector.h"
#include "block.h"
#include <vector>

#pragma once

class ChunkModel {
	public:
		struct Vertex {
			Vertex(int a = 69, int b = 69, int c = 69, float u = 0, float v = 0) {x=a;y=b;z=c;};
			float x, y, z;
			float u, v;
			// TODO: Texture, Colour, Lighting, Etc.
		};

		struct Face {
			Face(int a = 0, int b = 0, int c = 0) {v=a;vv=b;vvv=c;};
			uint16_t v, vv, vvv;
		};
		std::vector<Vertex> vertices;
		std::vector<Face> faces;

		void Build(blocktype_t blocks[]);
};