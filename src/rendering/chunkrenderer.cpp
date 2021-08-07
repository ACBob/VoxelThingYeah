#include "chunkrenderer.h"
#include "chunkmodel.h"

#include <glad/glad.h>

#include <memory>

ChunkRenderer::ChunkRenderer(ChunkModel *mdl)
{
	_mdl = mdl;
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mdl->vertices.size() * sizeof(ChunkModel::Vertex), mdl->vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mdl->faces.size() * sizeof(ChunkModel::Face), mdl->faces.data(), GL_DYNAMIC_DRAW);

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

void ChunkRenderer::Populate()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, _mdl->vertices.size() * sizeof(ChunkModel::Vertex), _mdl->vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _mdl->faces.size() * sizeof(ChunkModel::Face), _mdl->faces.data(), GL_DYNAMIC_DRAW);
}

ChunkRenderer::~ChunkRenderer()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void ChunkRenderer::Render()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glDrawElements(GL_TRIANGLES, _mdl->faces.size() * sizeof(ChunkModel::Face), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}