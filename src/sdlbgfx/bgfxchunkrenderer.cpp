#include "../chunkrenderer.h"

#include <bgfx/bgfx.h>

#include <memory>

ChunkRenderer::ChunkRenderer(ChunkModel *mdl)
{
	_mdl = mdl;
}

void ChunkRenderer::Populate()
{
	// TODO: Not create this every time
	bgfx::VertexLayout layout;
	layout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
	.end();

	ChunkModel::Vertex TMP1[_mdl->vertices.size()];
	for (int i = 0; i < _mdl->vertices.size(); i++)
		TMP1[i] = _mdl->vertices[i];
	ChunkModel::Face TMP2[_mdl->faces.size()];
	for (int i = 0; i < _mdl->faces.size(); i++)
		TMP2[i] = _mdl->faces[i];

    vtx = bgfx::createVertexBuffer(bgfx::makeRef(TMP1, sizeof(TMP1)), layout).idx;
    idx = bgfx::createIndexBuffer(bgfx::makeRef(TMP2, sizeof(TMP2))).idx;
}

ChunkRenderer::~ChunkRenderer()
{
	bgfx::VertexBufferHandle a;
	bgfx::IndexBufferHandle b;
	a.idx = vtx;
	b.idx = idx;

	bgfx::destroy(a);
	bgfx::destroy(b);
}

void ChunkRenderer::Render(shader_t shaderPrg)
{
	bgfx::VertexBufferHandle a;
	bgfx::IndexBufferHandle b;
	a.idx = vtx;
	b.idx = idx;
	bgfx::setVertexBuffer(0, a);
	bgfx::setIndexBuffer(b);

	bgfx::ProgramHandle g;
	g.idx = shaderPrg;
	
	bgfx::submit(0, g);
}