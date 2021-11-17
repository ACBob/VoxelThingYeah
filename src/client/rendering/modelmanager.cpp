#include "modelmanager.hpp"
#include "utility/direction.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <memory>

#include "modelloader.hpp"

std::vector<CModel *> modelSystem::loadedModels;

CModel::CModel( std::vector<Vertex> verts, std::vector<Face> faces ) : m_vertices( verts ), m_faces( faces )
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

	Update();
}

CModel::~CModel() { 
	glDeleteVertexArrays( 1, &m_iVao );
	glDeleteBuffers( 1, &m_iVbo );
	glDeleteBuffers( 1, &m_iEbo );
 }

void CModel::Update()
{

	glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
	glBufferData( GL_ARRAY_BUFFER, m_vertices.size() * sizeof( CModel::Vertex ), m_vertices.data(), GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iEbo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_faces.size() * sizeof( CModel::Face ), m_faces.data(), GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void CModel::Render()
{
	// Don't waste time trying to render
	if ( m_vertices.size() == 0 || m_faces.size() == 0 )
		return;
	// If we're not visible, don't bother
	if ( !m_bVisible )
		return;

	m_pShader->Use();

	glm::mat4 model = glm::mat4( 1.0f );
	model			= glm::translate( model, { m_vPosition.x, m_vPosition.y, m_vPosition.z } );
	model			= glm::scale( model, { m_vSize.x, m_vSize.y, m_vSize.z } );
	model			= glm::rotate( glm::rotate( glm::rotate( model, (float)( m_vRotation.z * DEG2RAD ), { 0, 0, -1 } ),
										(float)( m_vRotation.y * DEG2RAD ), { 0, -1, 0 } ),
						   (float)( m_vRotation.x * DEG2RAD ), { 1, 0, 0 } );

	m_pShader->SetMat4( "model", model );
	m_pShader->SetMat3( "normalMat", glm::mat3( glm::transpose( glm::inverse( model ) ) ) );
	m_pShader->SetVec4( "texCoordOffset", m_vUvOffset );
	m_pShader->SetVec4( "lighting", (Vector4f)m_vLighting );

	if ( m_pTex != nullptr )
		glBindTexture( GL_TEXTURE_2D, m_pTex->m_iId );

	glBindVertexArray( m_iVao );

	glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iEbo );

	glDrawElements( GL_TRIANGLES, m_faces.size() * sizeof( CModel::Face ), GL_UNSIGNED_INT, 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	glBindVertexArray( 0 );

	if ( m_pTex != nullptr )
		glBindTexture( GL_TEXTURE_2D, 0 );
}

void CModel::SetShader( CShader *shader ) { this->m_pShader = shader; }
CShader *CModel::GetShader() { return m_pShader; }
void CModel::SetTexture( CTexture *tex ) { this->m_pTex = tex; }
CTexture *CModel::GetTexture() { return m_pTex; }

CModel *modelSystem::LoadModel( const char *fp )
{
	CModel *m = new CModel();
	loadedModels.push_back( m );

	BOBJLoadModel( m, fp );

	return m;
}

void modelSystem::Init() {}
void modelSystem::UnInit()
{
	for ( CModel *m : loadedModels )
		delete m;
}

const std::vector<std::vector<int>> cubeTris = {
	{ 0, 1, 2, 3 }, // N
	{ 5, 0, 3, 6 }, // E
	{ 4, 5, 6, 7 }, // S
	{ 1, 4, 7, 2 }, // W

	{ 5, 4, 1, 0 }, // UP
	{ 3, 2, 7, 6 }	// DN
};
const CModel::Vertex cubeVertices[] = {
	// NORTH +Z
	{ 0.5f, 0.5f, 0.5f },
	{ -0.5f, 0.5f, 0.5f },
	{ -0.5f, -0.5f, 0.5f },
	{ 0.5f, -0.5f, 0.5f },
	// SOUTH -Z
	{ -0.5f, 0.5f, -0.5f },
	{ 0.5f, 0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f, -0.5f },
};

// Basically a hobbled together clone of ChunkModel's face supplying stuff
void GetCubeModel( CModel &m, Vector3f size, Vector4f uv )
{
	std::vector<CModel::Vertex> vertices = {};
	std::vector<CModel::Face> faces		 = {};

	for ( int j = 0; j < 6; j++ )
	{
		Vector3f normal = DirectionVector[j];

		for ( int i = 0; i < 4; i++ )
		{
			int k = vertices.size();
			vertices.push_back( cubeVertices[cubeTris[j][i]] );

			vertices[k].nx = normal.x;
			vertices[k].ny = normal.y;
			vertices[k].nz = normal.z;

			vertices[k].x *= size.x;
			vertices[k].y *= size.y;
			vertices[k].z *= size.z;

			switch ( i )
			{
				case 0:
					vertices[k].u = uv.x;
					vertices[k].v = uv.y;
					break;
				case 1:
					vertices[k].u = uv.z;
					vertices[k].v = uv.y;
					break;
				case 2:
					vertices[k].u = uv.z;
					vertices[k].v = uv.w;
					break;
				case 3:
					vertices[k].u = uv.x;
					vertices[k].v = uv.w;
					break;
			}
		}

		int nVertices = vertices.size();
		faces.push_back( { nVertices - 4, nVertices - 3, nVertices - 2 } );
		faces.push_back( { nVertices - 4, nVertices - 2, nVertices - 1 } );
	}

	m.m_vertices = vertices;
	m.m_faces	 = faces;
	m.Update();
}