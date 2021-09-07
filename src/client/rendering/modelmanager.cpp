#include "modelmanager.hpp"
#include "utility/direction.hpp"

#include "modelloader.hpp"

std::vector<CModel *> modelSystem::loadedModels;

CModel::CModel( std::vector<Vertex> verts, std::vector<Face> faces ) : vertices( verts ), faces( faces )
{
	renderer = new CModelRenderer();
}

CModel::~CModel() { delete renderer; }

void CModel::Update() { renderer->Populate( this ); }

void CModel::Render()
{
	// Don't waste time trying to render
	if ( vertices.size() == 0 || faces.size() == 0 )
		return;

	shader->Use();
	renderer->Render( position, rotation, size, shader, tex );
}

void CModel::SetShader( CShader *shader ) { this->shader = shader; }
CShader *CModel::GetShader() { return shader; }
void CModel::SetTexture( CTexture *tex ) { this->tex = tex; }
CTexture *CModel::GetTexture() { return tex; }

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
	}

	m.vertices = vertices;
	m.faces	   = faces;
	m.Update();
}