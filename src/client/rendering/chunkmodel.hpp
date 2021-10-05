#include "modelmanager.hpp"
#include "utility/vector.hpp"
#include "world/block.hpp"
#include <vector>

#include "assorted.hpp"

#pragma once

class CChunkRenderer; // Forward Decl.
class CChunkModel;

void BuildChunkModel( CChunkModel &mdl, CChunkModel &wmdl, CBlock blocks[], CVector pos, void *chunk );

class CChunkModel : public CModel
{
	DeclBaseClass(CModel);

	public:
		struct Vertex
		{
			float x, y, z;	  // Position
			float nx, ny, nz; // Normal
			float u, v;		  // Texture U/V
			float r, g, b, s; // Red/Green/Blue/Sky lighting
		};

		CChunkModel( std::vector<Vertex> verts = {}, std::vector<Face> faces = {} );

		void Update();
		void Render();

		std::vector<Vertex> m_vertices;
		std::vector<Face> m_faces;

	protected:
		CChunkRenderer *m_pRenderer = nullptr;
};

class CChunkRenderer : public CModelRenderer
{
  public:
	CChunkRenderer();
	~CChunkRenderer() {
		CModelRenderer::~CModelRenderer();
	};

	void Populate(void *_mdl);
};