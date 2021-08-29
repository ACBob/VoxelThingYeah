#include "shadermanager.hpp"

#include "utility/vector.hpp"

#pragma once

class ModelRenderer {
	public:
		ModelRenderer();
		~ModelRenderer();
		// Updated the index buffer and vertex buffer
		void Populate(void *mdl);

		void Render(Vector pos, Vector rot, Shader *shader);
	
	private:
		// Vertex Buffer
		unsigned int vbo;
		// Array buffer
		unsigned int vao;
		// Element buffer
		unsigned int ebo;

		// Holds the amount of faces/vertices we have
		std::size_t nFaces;
		std::size_t nVertices;
};