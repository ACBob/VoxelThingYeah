#include "chunkrenderer.h"

#include <glad/glad.h>

#include <memory>

ChunkRenderer::ChunkRenderer(ChunkModel *mdl)
{
	_mdl = mdl;
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mdl->vertices), &mdl->vertices[0], GL_DYNAMIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coordinate
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void ChunkRenderer::Populate()
{

}

ChunkRenderer::~ChunkRenderer()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void ChunkRenderer::Render()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(_mdl->faces.size()));
}