#include "rendering/chunkmodel.hpp"

#include "world/chunk.hpp" // CHUNKSIZE definitions
#include "world/world.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

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

std::vector<CModel::Vertex> sampleCubeFace( Direction dir, CBlock block, int x, int y, int z, int lr, int lg, int lb,
											int ls )
{
	std::vector<CModel::Vertex> g;
	for ( int i = 0; i < 4; i++ )
	{
		g.push_back( cubeVertices[cubeTris[dir][i]] );

		g[i].x += x;
		g[i].y += y;
		g[i].z += z;

		g[i].r = lr / 16.0f;
		g[i].g = lg / 16.0f;
		g[i].b = lb / 16.0f;
		g[i].a = ls / 16.0f;

		CVector normal = DirectionVector[dir];

		g[i].nx = normal.x;
		g[i].ny = normal.y;
		g[i].nz = normal.z;

		BlockTexture tex = block.GetSideTexture( dir );

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

std::vector<CModel::Vertex> samplePlant( CBlock block, int x, int y, int z, int lr, int lg, int lb, int ls )
{
	std::vector<CModel::Vertex> g;
	for ( int i = 0; i < 8; i++ )
	{
		g.push_back( plantVertices[plantTris[i]] );

		g[i].x += x;
		g[i].y += y;
		g[i].z += z;

		g[i].r = lr / 16.0f;
		g[i].g = lg / 16.0f;
		g[i].b = lb / 16.0f;
		g[i].a = ls / 16.0f;

		CVector normal = DirectionVector[i > 4 ? SOUTH : NORTH];

		// TODO:
		g[i].nx = normal.x;
		g[i].ny = normal.y;
		g[i].nz = normal.z;

		BlockTexture tex = block.GetSideTexture( NORTH );

		switch ( i )
		{
			case 0:
			case 4:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
				break;
			case 1:
			case 5:
				g[i].u = (float)( tex.x + tex.sizex ) / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
				break;
			case 2:
			case 6:
				g[i].u = (float)( tex.x + tex.sizex ) / 16.0f;
				g[i].v = (float)( tex.y + tex.sizey ) / 16.0f;
				break;
			case 3:
			case 7:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)( tex.y + tex.sizey ) / 16.0f;
				break;
		}
	}

	return g;
}

// We include the chunk manager here so we can test our neighbouring chunks
void BuildChunkModel( CModel &mdl, CModel &wmdl, CBlock blocks[], CVector pos, void *chunk )
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
				CBlock block = blocks[CHUNK3D_TO_1D( x, y, z )];

				// block here! Construct!
				if ( block.m_iBlockType != AIR )
				{
					BlockFeatures blockFeatures = GetBlockFeatures( block.m_iBlockType );
					switch ( blockFeatures.model )
					{
						case BLOCKMODEL_CUBE:
							for ( int i = 0; i < 6; i++ )
							{
								Colour colour;

								CVector neighbour = CVector( x, y, z ) + DirectionVector[i];
								if ( ValidChunkPosition( neighbour ) )
								{
									blocktype_t blockType =
										reinterpret_cast<CChunk *>( chunk )->GetBlockAtLocal( neighbour )->m_iBlockType;
									BlockFeatures bF = GetBlockFeatures( blockType );
									if ( bF.rule == OBSCURERULE_ALWAYS ||
										 ( bF.rule == OBSCURERULE_SIMILAR && blockType == block.m_iBlockType ) )
										continue;

									colour = reinterpret_cast<CChunk *>( chunk )->GetLightingLocal( neighbour );
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

										CBlock *b = chunkNeighbour->GetBlockAtLocal( neighbour );
										if ( b == nullptr )
											continue;
										BlockFeatures bF = GetBlockFeatures( b->m_iBlockType );
										if ( bF.rule == OBSCURERULE_ALWAYS ||
											 ( bF.rule == OBSCURERULE_SIMILAR &&
											   b->m_iBlockType == block.m_iBlockType ) )
											continue;

										colour = chunkNeighbour->GetLightingLocal( neighbour );
									}
								}

								std::vector<CModel::Vertex> g =
									sampleCubeFace( Direction( i ), block, x, y, z, (int)colour.x, (int)colour.y,
													(int)colour.z, (int)colour.w );

								if ( block.m_iBlockType == WATER || block.m_iBlockType == WATERSRC )
								{
									std::copy( g.begin(), g.end(), std::back_inserter( wmdl.m_vertices ) );

									int nVertices = wmdl.m_vertices.size();

									wmdl.m_faces.push_back( { nVertices - 4, nVertices - 3, nVertices - 2 } );
									wmdl.m_faces.push_back( { nVertices - 4, nVertices - 2, nVertices - 1 } );

									if ( i == UP )
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
							break;

						case BLOCKMODEL_PLANT:
							CVector colour =
								reinterpret_cast<CChunk *>( chunk )->GetLightingLocal( CVector( x, y, z ) );
							std::vector<CModel::Vertex> g =
								samplePlant( block, x, y, z, colour.x, colour.y, colour.z, colour.w );
							std::copy( g.begin(), g.end(), std::back_inserter( mdl.m_vertices ) );

							int nVertices = mdl.m_vertices.size();

							mdl.m_faces.push_back( { nVertices - 4, nVertices - 3, nVertices - 2 } );
							mdl.m_faces.push_back( { nVertices - 4, nVertices - 2, nVertices - 1 } );
							mdl.m_faces.push_back( { nVertices - 2, nVertices - 3, nVertices - 4 } );
							mdl.m_faces.push_back( { nVertices - 1, nVertices - 2, nVertices - 4 } );

							mdl.m_faces.push_back( { nVertices - 8, nVertices - 7, nVertices - 6 } );
							mdl.m_faces.push_back( { nVertices - 8, nVertices - 6, nVertices - 5 } );
							mdl.m_faces.push_back( { nVertices - 6, nVertices - 7, nVertices - 8 } );
							mdl.m_faces.push_back( { nVertices - 5, nVertices - 6, nVertices - 8 } );
							break;
					}
				}
			}
		}
	}

	mdl.Update();
	wmdl.Update();
}