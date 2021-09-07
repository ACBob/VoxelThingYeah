#include "shadermanager.hpp"

#include "utility/vector.hpp"

#include "texturemanager.hpp"

#pragma once

class CModelRenderer
{
  public:
	CModelRenderer();
	~CModelRenderer();
	// Updated the index buffer and vertex buffer
	void Populate( void *mdl );

	void Render( CVector pos, CVector rot, CVector size, CShader *shader, CTexture *tex );

  private:
	// Vertex Buffer
	unsigned int m_iVbo;
	// Array buffer
	unsigned int m_iVao;
	// Element buffer
	unsigned int m_iEbo;

	// Holds the amount of faces/vertices we have
	std::size_t m_nFaces;
	std::size_t m_nVertices;
};