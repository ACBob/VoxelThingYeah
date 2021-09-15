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
	m_pRenderer->Render( m_vPosition, m_vRotation, m_vSize, m_pShader, m_pTex );
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

void GetCubeModel( CModel &m, CVector size )
{
	std::vector<CModel::Vertex> vertices = {
		// NORTH +Z
		{ size.x, size.y, size.z },
		{ -size.x, size.y, size.z },
		{ -size.x, -size.y, size.z },
		{ size.x, -size.y, size.z },
		// SOUTH -Z
		{ -size.x, size.y, -size.z },
		{ size.x, size.y, -size.z },
		{ size.x, -size.y, -size.z },
		{ -size.x, -size.y, -size.z },
	};
	std::vector<CModel::Face> faces = { // +Z
										{ 2, 1, 0 },
										{ 3, 2, 0 },

										// -X
										{ 3, 0, 5 },
										{ 6, 3, 5 },

										// -Z
										{ 6, 5, 4 },
										{ 7, 6, 4 },

										// +X
										{ 7, 4, 1 },
										{ 2, 7, 1 },

										// +Y
										{ 1, 4, 5 },
										{ 0, 1, 5 },

										// -Y
										{ 7, 2, 3 },
										{ 6, 7, 3 } };
	for ( int i = 0; i < 6; i += 2 )
	{
		CVector normal			  = DirectionVector[i];
		vertices[faces[i].v].nx	  = normal.x;
		vertices[faces[i].v].ny	  = normal.y;
		vertices[faces[i].v].nz	  = normal.z;
		vertices[faces[i].vv].nx  = normal.x;
		vertices[faces[i].vv].ny  = normal.y;
		vertices[faces[i].vv].nz  = normal.z;
		vertices[faces[i].vvv].nx = normal.x;
		vertices[faces[i].vvv].ny = normal.y;
		vertices[faces[i].vvv].nz = normal.z;

		vertices[faces[i].v].u = 0.0f;
		vertices[faces[i].v].v = 0.0f;
		vertices[faces[i].vv].u = 1.0f;
		vertices[faces[i].vv].v = 0.0f;
		vertices[faces[i].vvv].u = 0.0f;
		vertices[faces[i].vvv].v = 1.0f;
	}

	m.m_vertices = vertices;
	m.m_faces	 = faces;
	m.Update();
}