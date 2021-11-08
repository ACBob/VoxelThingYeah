#include "shadermanager.hpp"

#include "utility/vector.hpp"

#include "texturemanager.hpp"

#include "colour.hpp"

#pragma once

class CModelRenderer
{
  public:
	CModelRenderer();
	~CModelRenderer();
	// Updated the index buffer and vertex buffer
	void Populate( void *mdl );

	void Render( Vector3f pos, Vector3f rot, Vector3f size, Vector4f uvOffset, CColour light, CShader *shader,
				 CTexture *tex );

  protected:
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