#include "modelmanager.hpp"
#include "utility/direction.hpp"

#include "modelloader.hpp"

std::vector<CModel *> modelSystem::loadedModels;

CModel::CModel( std::vector<Vertex> verts, std::vector<Face> faces ) : m_vertices( verts ), m_faces( faces )
{
	m_pRenderer = new CModelRenderer();
}

CModel::~CModel() { delete m_pRenderer; }

void CModel::Update() { m_pRenderer->Populate( this ); }

void CModel::Render()
{
	// Don't waste time trying to render
	if ( m_vertices.size() == 0 || m_faces.size() == 0 )
		return;
	// If we're not visible, don't bother
	if ( !m_bVisible )
		return;

	m_pShader->Use();
	m_pRenderer->Render( m_vPosition, m_vRotation, m_vSize, m_vUvOffset, m_vLighting, m_pShader, m_pTex );
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
	{ 3, 2, 1, 0 }, // N
	{ 6, 3, 0, 5 }, // E
	{ 7, 6, 5, 4 }, // S
	{ 2, 7, 4, 1 }, // W

	{ 0, 1, 4, 5 }, // UP
	{ 6, 7, 2, 3 }	// DN
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
void GetCubeModel( CModel &m, CVector size, CVector uv )
{
	std::vector<CModel::Vertex> vertices = {};
	std::vector<CModel::Face> faces		 = {};

	for ( int j = 0; j < 6; j++ )
	{
		CVector normal = DirectionVector[j];

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