#include "rendering/chunkmodel.hpp"

#include "world/chunk.hpp" // CHUNKSIZE definitions
#include "world/world.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

#include "block/blockbase.hpp"

#include "direction.hpp"

#include "blocktexture.hpp"

const CModel::Vertex cubeVertices[] = {
	// NORTH +Z
	{ BLOCKUNIT, BLOCKUNIT, BLOCKUNIT },
	{ 0, BLOCKUNIT, BLOCKUNIT },
	{ 0, 0, BLOCKUNIT },
	{ BLOCKUNIT, 0, BLOCKUNIT },
	// SOUTH -Z
	{ 0, BLOCKUNIT, 0 },
	{ BLOCKUNIT, BLOCKUNIT, 0 },
	{ BLOCKUNIT, 0, 0 },
	{ 0, 0, 0 },
};
const CModel::Vertex plantVertices[] = {
	{ 0, BLOCKUNIT, 0 },		 { BLOCKUNIT, 0, BLOCKUNIT }, { 0, 0, 0 },		   { BLOCKUNIT, BLOCKUNIT, BLOCKUNIT },

	{ BLOCKUNIT, BLOCKUNIT, 0 }, { 0, 0, BLOCKUNIT },		  { BLOCKUNIT, 0, 0 }, { 0, BLOCKUNIT, BLOCKUNIT },
};

// D-d-d-d-double vector
const std::vector<std::vector<int>> cubeTris = {
	{ 3, 2, 1, 0 }, // N
	{ 6, 3, 0, 5 }, // E
	{ 7, 6, 5, 4 }, // S
	{ 2, 7, 4, 1 }, // W

	{ 0, 1, 4, 5 }, // UP
	{ 6, 7, 2, 3 }	// DN
};

const std::vector<int> plantTris = { 2, 1, 3, 0, 6, 5, 7, 4 };

std::vector<CModel::Vertex> sampleCubeFace( Direction dir, BLOCKID block, int x, int y, int z, int lr, int lg, int lb,
											int ls, int cr, int cg, int cb )
{
	std::vector<CModel::Vertex> g;
	for ( int i = 0; i < 4; i++ )
	{
		g.push_back( cubeVertices[cubeTris[dir][i]] );

		g[i].x += x;
		g[i].y += y;
		g[i].z += z;

		g[i].lr = lr / 16.0f;
		g[i].lg = lg / 16.0f;
		g[i].lb = lb / 16.0f;
		g[i].la = ls / 16.0f;

		g[i].cr = cr / 16.0f;
		g[i].cg = cg / 16.0f;
		g[i].cb = cb / 16.0f;

		CVector normal = DirectionVector[dir];

		g[i].nx = normal.x;
		g[i].ny = normal.y;
		g[i].nz = normal.z;

		BlockTexture tex = GetDefaultBlockTextureSide( block, dir );

		switch ( i )
		{
			case 0:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
				break;
			case 1:
				g[i].u = (float)( tex.x + tex.sizex ) / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
				break;
			case 2:
				g[i].u = (float)( tex.x + tex.sizex ) / 16.0f;
				g[i].v = (float)( tex.y + tex.sizey ) / 16.0f;
				break;
			case 3:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)( tex.y + tex.sizey ) / 16.0f;
				break;
		}
	}

	return g;
}

// We include the chunk manager here so we can test our neighbouring chunks
// TODO: Rewrite entirely
// (This may be one of the oldest pieces of unrewritten code)
void BuildChunkModel( CModel &mdl, CModel &wmdl, BLOCKID blocks[], BLOCKVAL meta[], CVector pos, void *chunk )
{
	mdl.m_vertices.clear();
	mdl.m_faces.clear();
	wmdl.m_vertices.clear();
	wmdl.m_faces.clear();

	for ( int y = 0; y < CHUNKSIZE_Y; y++ )
	{
		for ( int x = 0; x < CHUNKSIZE_X; x++ )
		{
			for ( int z = 0; z < CHUNKSIZE_Z; z++ )
			{
				BLOCKID block = blocks[CHUNK3D_TO_1D( x, y, z )];

				// block here! Construct!
				if ( block != BLCK_AIR )
				{
					for ( int i = 0; i < 6; i++ )
					{
						Colour lightColour;
						Colour blockColouration(16,16,16);

						CVector neighbour = CVector( x, y, z ) + DirectionVector[i];
						if ( ValidChunkPosition( neighbour ) )
						{
							BLOCKID blockType = std::get<0>(reinterpret_cast<CChunk *>( chunk )->GetBlockAtLocal( neighbour ));
							if ( blockType != BLCK_NONE )
								continue;
						}
						else
						{
							// Test a neighbour
							CChunk *chunkNeighbour =
								reinterpret_cast<CChunk *>( chunk )->Neighbour( Direction( i ) );
							if ( chunkNeighbour != nullptr )
							{
								neighbour = neighbour + ( DirectionVector[i] *
															CVector( -CHUNKSIZE_X, -CHUNKSIZE_Y, -CHUNKSIZE_Z ) );

								BLOCKID b = std::get<0>(chunkNeighbour->GetBlockAtLocal( neighbour ));
								if ( b != BLCK_NONE )
									continue;
							}
						}

						std::vector<CModel::Vertex> g =
							sampleCubeFace( Direction( i ), block, x, y, z, (int)lightColour.x, (int)lightColour.y,
											(int)lightColour.z, (int)lightColour.w, (int)blockColouration.x, (int)blockColouration.y, (int)blockColouration.z );

						if ( block == BLCK_WATER || block == BLCK_WATERSRC )
						{
							std::copy( g.begin(), g.end(), std::back_inserter( wmdl.m_vertices ) );

							int nVertices = wmdl.m_vertices.size();

							wmdl.m_faces.push_back( { nVertices - 4, nVertices - 3, nVertices - 2 } );
							wmdl.m_faces.push_back( { nVertices - 4, nVertices - 2, nVertices - 1 } );

							if ( i == UP ) // Double the up faces for water
							{
								wmdl.m_faces.push_back( { nVertices - 2, nVertices - 3, nVertices - 4 } );
								wmdl.m_faces.push_back( { nVertices - 1, nVertices - 2, nVertices - 4 } );
							}
						}
						else
						{
							std::copy( g.begin(), g.end(), std::back_inserter( mdl.m_vertices ) );

							int nVertices = mdl.m_vertices.size();

							mdl.m_faces.push_back( { nVertices - 4, nVertices - 3, nVertices - 2 } );
							mdl.m_faces.push_back( { nVertices - 4, nVertices - 2, nVertices - 1 } );
						}
					}
				}
			}
		}
	}

	mdl.Update();
	wmdl.Update();
}