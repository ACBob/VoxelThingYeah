#include "chunk.hpp"

CClientChunk::CClientChunk() {}
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

void CClientChunk::ConstructModel()
{
	m_model->m_vertices.clear();
	m_model->m_faces.clear();

	for ( int z = 0; z < CHUNKSIZE_Z; z++ )
	{
		for ( int y = 0; y < CHUNKSIZE_Y; y++ )
		{
			for ( int x = 0; x < CHUNKSIZE_X; x++ )
			{
				if ( Get )
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
						Model::Vertex v;
						v.x = m_pos.x * m_size.x + x + cubeVertices[cubeTriangles[face][i]][0];
						v.y = m_pos.y * m_size.y + y + cubeVertices[cubeTriangles[face][i]][1];
						v.z = m_pos.z * m_size.z + z + cubeVertices[cubeTriangles[face][i]][2];
						v.r = qBound(0.1f, fmaxf( red / 16.0f, sky / 16.0f ) * 1.5f, 1.0f);
						v.g = qBound(0.1f, fmaxf( green / 16.0f, sky / 16.0f ) * 1.5f, 1.0f);
						v.b = qBound(0.1f, fmaxf( blue / 16.0f, sky / 16.0f ) * 1.5f, 1.0f);

						Vector4f uv;
						if ( m_editorState != nullptr && m_editorState->m_pBlockDefs != nullptr )
						{
							uv = render::getUV( m_editorState->m_pBlockDefs, getID( x, y, z ) );
							uv.y -= 1 / 16.0f; // HACK: otherwise all textures are offset by 1/16
							uv.w -= 1 / 16.0f;
						}
						else
							uv = { 0, 0, 1, 1 };

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

						m_model->m_vertices.push_back( v );
					}

					m_model->m_faces.push_back( Model::Face{ m_model->m_vertices.size() - 4,
															 m_model->m_vertices.size() - 3,
															 m_model->m_vertices.size() - 2 } );
					m_model->m_faces.push_back( Model::Face{ m_model->m_vertices.size() - 4,
															 m_model->m_vertices.size() - 2,
															 m_model->m_vertices.size() - 1 } );
				}
			}
		}
	}

	m_model->update();
}