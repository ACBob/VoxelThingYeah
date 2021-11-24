#include "rendering/chunkmodel.hpp"

#include "world/chunk.hpp" // CHUNKSIZE definitions
#include "world/world.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

#include "blocks/blockbase.hpp"

#include "colour.hpp"

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
	{ 0, 1, 2, 3 }, // N
	{ 5, 0, 3, 6 }, // E
	{ 4, 5, 6, 7 }, // S
	{ 1, 4, 7, 2 }, // W

	{ 5, 4, 1, 0 }, // UP
	{ 3, 2, 7, 6 }	// DN
};

const std::vector<int> plantTris = { 4, 7, 5, 6, 0, 3, 1, 2 };

std::vector<CModel::Vertex> sampleCubeFace( Direction dir, block_t block, int x, int y, int z, CColour light,
											CColour tint )
{
	std::vector<CModel::Vertex> g;
	for ( int i = 0; i < 4; i++ )
	{
		g.push_back( cubeVertices[cubeTris[dir][i]] );

		g[i].x += x;
		g[i].y += y;
		g[i].z += z;

		g[i].lr = light.r / 255.0f;
		g[i].lg = light.g / 255.0f;
		g[i].lb = light.b / 255.0f;
		g[i].la = light.a / 255.0f;

		g[i].cr = tint.r / 255.0f;
		g[i].cg = tint.g / 255.0f;
		g[i].cb = tint.b / 255.0f;

		Vector3f normal = DirectionVector[dir];

		g[i].nx = normal.x;
		g[i].ny = normal.y;
		g[i].nz = normal.z;

		BlockTexture tex = BlockType( block.GetType() ).GetTexture( dir, block.GetMeta() );

		switch ( i )
		{
			case 0:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
				break;
			case 1:
				g[i].u = (float)( tex.x + tex.z ) / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
				break;
			case 2:
				g[i].u = (float)( tex.x + tex.z ) / 16.0f;
				g[i].v = (float)( tex.y + tex.w ) / 16.0f;
				break;
			case 3:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)( tex.y + tex.w ) / 16.0f;
				break;
		}
	}

	return g;
}

std::vector<CModel::Vertex> samplePlant( Direction dir, block_t block, int x, int y, int z, CColour light,
											CColour tint )
{
	std::vector<CModel::Vertex> g;
	for ( int i = 0; i < 8; i++ )
	{
		g.push_back( plantVertices[plantTris[i]] );

		g[i].x += x;
		g[i].y += y;
		g[i].z += z;

		g[i].lr = light.r / 255.0f;
		g[i].lg = light.g / 255.0f;
		g[i].lb = light.b / 255.0f;
		g[i].la = light.a / 255.0f;

		g[i].cr = tint.r / 255.0f;
		g[i].cg = tint.g / 255.0f;
		g[i].cb = tint.b / 255.0f;

		Vector3f normal = DirectionVector[dir];

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
				g[i].u = (float)( tex.x + tex.z ) / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
				break;
			case 2:
			case 6:
				g[i].u = (float)( tex.x + tex.z ) / 16.0f;
				g[i].v = (float)( tex.y + tex.w ) / 16.0f;
				break;
			case 3:
			case 7:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)( tex.y + tex.w ) / 16.0f;
				break;
		}
	}

	return g;
}

// We include the chunk manager here so we can test our neighbouring chunks
void BuildChunkModel( CModel &mdl, CModel &wmdl, block_t blocks[], Vector3f pos, CChunk *chunk )
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
				block_t block = blocks[CHUNK3D_TO_1D( x, y, z )];

				// block here! Construct!
				if ( block.GetType() != AIR )
				{
					// Model
					BLOCKMODEL model = BlockType( block.GetType() ).GetModel( chunk, { (float)x, (float)y, (float)z }, block.GetMeta() );

					switch (model)
					{
						default:
						case BLOCKMODEL_CUBE: {
							for ( int i = 0; i < 6; i++ )
							{
								CColour lightColour;
								CColour blockColouration =
									BlockType( block.GetType() ).GetTint( chunk, pos, block.GetMeta(), (Direction)i );

								Vector3f neighbour = Vector3f( x, y, z ) + DirectionVector[i];
								if ( ValidChunkPosition( neighbour ) )
								{
									BLOCKID blockType = chunk->GetBlockAtLocal( neighbour )->GetType();

									if ( !BlockType( block.GetType() ).FaceVisible( (Direction)i, blockType ) )
										continue;

									lightColour = chunk->GetLightingLocal( neighbour );
								}
								else
								{
									// Test a neighbour
									CChunk *chunkNeighbour = chunk->Neighbour( Direction( i ) );
									if ( chunkNeighbour != nullptr )
									{
										neighbour = neighbour + ( DirectionVector[i] *
																Vector3f( -CHUNKSIZE_X, -CHUNKSIZE_Y, -CHUNKSIZE_Z ) );

										block_t *b = chunkNeighbour->GetBlockAtLocal( neighbour );
										if ( b == nullptr )
											continue;
										if ( !BlockType( block.GetType() ).FaceVisible( (Direction)i, b->GetType() ) )
											continue;

										lightColour = chunkNeighbour->GetLightingLocal( neighbour );
									}
								}

								std::vector<CModel::Vertex> g =
									sampleCubeFace( Direction( i ), block, x, y, z, lightColour, blockColouration );

								if ( block.GetType() == WATER || block.GetType() == WATERSRC )
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
						}
						break;
						case BLOCKMODEL_CROSS: {
							// No need to test neighbours, we're not a full block so we don't (can't?) care
							std::vector<CModel::Vertex> g = samplePlant( UP, block, x, y, z, chunk->GetLightingLocal( pos ), BlockType( block.GetType() ).GetTint( chunk, pos, block.GetMeta(), UP ) );
							
							std::copy( g.begin(), g.end(), std::back_inserter( mdl.m_vertices ) );

							int nVertices = mdl.m_vertices.size();

							// Crosses double the faces so are visible from both sides
							mdl.m_faces.push_back( { nVertices - 4, nVertices - 3, nVertices - 2 } );
							mdl.m_faces.push_back( { nVertices - 4, nVertices - 2, nVertices - 1 } );
							mdl.m_faces.push_back( { nVertices - 2, nVertices - 3, nVertices - 4 } );
							mdl.m_faces.push_back( { nVertices - 1, nVertices - 2, nVertices - 4 } );

							mdl.m_faces.push_back( { nVertices - 8, nVertices - 7, nVertices - 6 } );
							mdl.m_faces.push_back( { nVertices - 8, nVertices - 6, nVertices - 5 } );
							mdl.m_faces.push_back( { nVertices - 6, nVertices - 7, nVertices - 8 } );
							mdl.m_faces.push_back( { nVertices - 5, nVertices - 6, nVertices - 8 } );

						}
					}
				}
			}
		}
	}

	mdl.Update();
	wmdl.Update();
}