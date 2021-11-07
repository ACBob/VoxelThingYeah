#include "modelrenderer.hpp"
#include "chunkmodel.hpp"
#include "modelmanager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

#include <memory>

CModelRenderer::CModelRenderer() : m_nFaces( 0 ), m_nVertices( 0 )
{
	glGenVertexArrays( 1, &m_iVao );
	glGenBuffers( 1, &m_iVbo );
	glGenBuffers( 1, &m_iEbo );

	glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iEbo );

	glBindVertexArray( m_iVao );

	// Position
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, 15 * sizeof( float ), (void *)offsetof( CModel::Vertex, x ) );
	glEnableVertexAttribArray( 0 );
	// Normal
	glVertexAttribPointer( 1, 3, GL_FLOAT, false, 15 * sizeof( float ), (void *)offsetof( CModel::Vertex, nx ) );
	glEnableVertexAttribArray( 1 );
	// texture coordinate
	glVertexAttribPointer( 2, 2, GL_FLOAT, false, 15 * sizeof( float ), (void *)offsetof( CModel::Vertex, u ) );
	glEnableVertexAttribArray( 2 );
	// Vertex CColour
	glVertexAttribPointer( 3, 3, GL_FLOAT, false, 15 * sizeof( float ), (void *)offsetof( CModel::Vertex, cr ) );
	glEnableVertexAttribArray( 3 );
	// Vertex Lighting
	glVertexAttribPointer( 4, 3, GL_FLOAT, false, 15 * sizeof( float ), (void *)offsetof( CModel::Vertex, lr ) );
	glEnableVertexAttribArray( 4 );

	glBindVertexArray( 0 );
}

void CModelRenderer::Populate( void *_mdl )
{
	CModel *mdl = reinterpret_cast<CModel *>( _mdl );

	m_nVertices = mdl->m_vertices.size();
	m_nFaces	= mdl->m_faces.size();

	glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
	glBufferData( GL_ARRAY_BUFFER, m_nVertices * sizeof( CModel::Vertex ), mdl->m_vertices.data(), GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iEbo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_nFaces * sizeof( CModel::Face ), mdl->m_faces.data(), GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

CModelRenderer::~CModelRenderer()
{
	glDeleteVertexArrays( 1, &m_iVao );
	glDeleteBuffers( 1, &m_iVbo );
	glDeleteBuffers( 1, &m_iEbo );
}

void CModelRenderer::Render( CVector pos, CVector rot, CVector size, CVector uvOffset, CColour light, CShader *shader,
							 CTexture *tex )
{

	glm::mat4 model = glm::mat4( 1.0f );
	model			= glm::translate( model, { pos.x, pos.y, pos.z } );
	model			= glm::scale( model, { size.x, size.y, size.z } );
	model			= glm::rotate( glm::rotate( glm::rotate( model, (float)( rot.z * DEG2RAD ), { 0, 0, -1 } ),
										(float)( rot.y * DEG2RAD ), { 0, -1, 0 } ),
						   (float)( rot.x * DEG2RAD ), { 1, 0, 0 } );

	shader->SetMat4( "model", model );
	shader->SetMat3( "normalMat", glm::mat3( glm::transpose( glm::inverse( model ) ) ) );
	shader->SetVec4( "texCoordOffset", uvOffset );
	CVector vLight = light;
	vLight = vLight / 255.0f;
	shader->SetVec4( "lighting", vLight );

	if ( tex != nullptr )
		glBindTexture( GL_TEXTURE_2D, tex->m_iId );

	glBindVertexArray( m_iVao );

	glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iEbo );

	glDrawElements( GL_TRIANGLES, m_nFaces * sizeof( CModel::Face ), GL_UNSIGNED_INT, 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	glBindVertexArray( 0 );

	if ( tex != nullptr )
		glBindTexture( GL_TEXTURE_2D, 0 );

	// model = glm::mat4(1.0f);
	// shader->SetMat4("model", model);
}