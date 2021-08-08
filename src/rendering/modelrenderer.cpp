#include "modelrenderer.h"
#include "model.h"
#include "chunkmodel.h"

#include <glad/glad.h>

#include <memory>

ModelRenderer::ModelRenderer() :
	nFaces(0),
	nVertices(0)
{	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)offsetof(ChunkModel::Vertex, x));
	glEnableVertexAttribArray(0);
	// Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)offsetof(ChunkModel::Vertex, nx));
	glEnableVertexAttribArray(1);
	// texture coordinate
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)offsetof(ChunkModel::Vertex, u));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void ModelRenderer::Populate(void *_mdl)
{
	Model *mdl = reinterpret_cast<Model*>(_mdl);

	nVertices = mdl->vertices.size();
	nFaces = mdl->faces.size();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(Model::Vertex), mdl->vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nFaces * sizeof(Model::Face), mdl->faces.data(), GL_DYNAMIC_DRAW);
}

ModelRenderer::~ModelRenderer()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void ModelRenderer::Render()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glDrawElements(GL_TRIANGLES, nFaces * sizeof(Model::Face), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}