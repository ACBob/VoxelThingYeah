#include <vector>

#include "utility/vector.hpp"

#include "shadermanager.hpp"
#include "texturemanager.hpp"

#include "modelrenderer.hpp"

#pragma once

class Model {
	public:
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

		Model(std::vector<Vertex> verts = {}, std::vector<Face> faces = {});
		~Model();


		void Update();
		void Render();

		void SetShader(Shader *shader);
		Shader *GetShader();

		std::vector<Vertex> vertices;
		std::vector<Face> faces;

		Vector position;
		Vector rotation;

		private:
			Shader *shader = nullptr;
			Texture *tex = nullptr;

			ModelRenderer *renderer = nullptr;
};

void GetCubeModel(Model &m, Vector size = Vector(1,1,1));