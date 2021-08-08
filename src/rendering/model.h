#include <vector>

#include "modelrenderer.h"
#include "utility/vector.h"

#pragma once

class Model {
	public:
		Model();
		void Update();
		void Render();

		void SetShader(Shader *shader);
		Shader *GetShader();

		struct Vertex {
			float x, y, z; // Position
			float nx, ny, nz; // Normal
			float u, v; // Texture U/V
			// TODO: Texture, Lighting, Etc.
		};

		struct Face {
			Face(int a = 0, int b = 0, int c = 0) {v=a;vv=b;vvv=c;};
			unsigned int v, vv, vvv;
		};
		std::vector<Vertex> vertices;
		std::vector<Face> faces;
		ModelRenderer renderer;

		Vector pos;
};

Model GetCubeModel(Vector size = Vector(1,1,1));