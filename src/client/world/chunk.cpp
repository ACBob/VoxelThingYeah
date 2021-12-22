#include "chunk.hpp"
#include "render/material.hpp"

CClientChunk::CClientChunk( int x, int y, int z, int sizeX, int sizeY, int sizeZ ) : CChunk(x, y, z, sizeX, sizeY, sizeZ)
{
	m_model.m_pShader = rendering::materials::GetNamedShader("generic");
	m_model.m_pTexture = rendering::materials::LoadTexture("terrain.png");
}
CClientChunk::~CClientChunk() {}

const int cubeVertices[8][3] = {
	// NORTH +Z
	{ 1, 1, 1 },
	{ 0, 1, 1 },
	{ 0, 0, 1 },
	{ 1, 0, 1 },
	// SOUTH -Z
	{ 0, 1, 0 },
	{ 1, 1, 0 },
	{ 1, 0, 0 },
	{ 0, 0, 0 },
};

const int cubeTriangles[6][4] = {
	{ 0, 1, 2, 3 }, // N
	{ 5, 0, 3, 6 }, // E
	{ 4, 5, 6, 7 }, // S
	{ 1, 4, 7, 2 }, // W

	{ 5, 4, 1, 0 }, // UP
	{ 3, 2, 7, 6 }	// DN
};

const int dir[6][3] = { { 0, 0, 1 }, { 1, 0, 0 }, { 0, 0, -1 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 } };

void CClientChunk::constructModel()
{
	m_model.m_vertices.clear();
	m_model.m_faces.clear();

	calculateLighting();

	for ( int z = 0; z < m_size.z; z++ )
	{
		for ( int y = 0; y < m_size.y; y++ )
		{
			for ( int x = 0; x < m_size.x; x++ )
			{
				if ( getID( x, y, z ) == 0 )
				{
					continue;
				}

				for ( int face = 0; face < 6; face++ )
				{
					// test if face is visible, by checking if there's a block in the direction of the face
					int dx = x + dir[face][0];
					int dy = y + dir[face][1];
					int dz = z + dir[face][2];

					if ( getID( dx, dy, dz ) != 0 )
					{
						continue;
					}

					// get lighting
					uint16_t light = getLighting( dx, dy, dz );
					uint8_t red = ( light >> 12 ) & 0xFF;
					uint8_t green = ( light >> 8 ) & 0xFF;
					uint8_t blue = ( light >> 4 ) & 0xFF;
					uint8_t sky = ( light >> 0 ) & 0xFF;

					// construct the face
					for ( int i = 0; i < 4; i++ )
					{
						rendering::models::CModel::Vertex v;
						v.x = m_pos.x * m_size.x + x + cubeVertices[cubeTriangles[face][i]][0];
						v.y = m_pos.y * m_size.y + y + cubeVertices[cubeTriangles[face][i]][1];
						v.z = m_pos.z * m_size.z + z + cubeVertices[cubeTriangles[face][i]][2];
						v.r = fmaxf( red / 16.0f, sky / 16.0f );
						v.g = fmaxf( green / 16.0f, sky / 16.0f );
						v.b = fmaxf( blue / 16.0f, sky / 16.0f );

						Vector4f uv;
						uv = { 0, 0, 1 / 16.0f, -1 / 16.0f };

						switch ( i )
						{
							case 0:
								v.u = uv.x;
								v.v = uv.y;
								break;
							case 1:
								v.u = uv.z;
								v.v = uv.y;
								break;
							case 2:
								v.u = uv.z;
								v.v = uv.w;
								break;
							case 3:
								v.u = uv.x;
								v.v = uv.w;
								break;
						}

						v.nx = dir[face][0];
						v.ny = dir[face][1];
						v.nz = dir[face][2];

						m_model.m_vertices.push_back( v );
					}

					m_model.m_faces.push_back( rendering::models::CModel::Face{ (vertexIndex_t)m_model.m_vertices.size() - 4,
															 (vertexIndex_t)m_model.m_vertices.size() - 3,
															 (vertexIndex_t)m_model.m_vertices.size() - 2 } );
					m_model.m_faces.push_back( rendering::models::CModel::Face{ (vertexIndex_t)m_model.m_vertices.size() - 4,
															 (vertexIndex_t)m_model.m_vertices.size() - 2,
															 (vertexIndex_t)m_model.m_vertices.size() - 1 } );
				}
			}
		}
	}

	m_model.Update();
}

void CClientChunk::render()
{
	m_model.Render(
		m_pos,
		{0,0,0},
		{1,1,1}
	);
}