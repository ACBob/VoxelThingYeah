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

const std::vector<int> plantTris = { 2, 1, 3, 0, 6, 5, 7, 4 };

std::vector<CModel::Vertex> sampleCubeFace( Direction dir, CBlock block, int x, int y, int z, CColour light,
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

		BlockTexture tex = BlockType( block.m_iBlockType ).GetTexture( dir, block.m_iBlockData );

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

std::vector<CModel::Vertex> samplePlant( CBlock block, int x, int y, int z, int lr, int lg, int lb, int ls )
{
	std::vector<CModel::Vertex> g;
	for ( int i = 0; i < 8; i++ )
	{
		g.push_back( plantVertices[plantTris[i]] );

		g[i].x += x;
		g[i].y += y;
		g[i].z += z;

		g[i].lr = lr / 16.0f;
		g[i].lg = lg / 16.0f;
		g[i].lb = lb / 16.0f;
		g[i].la = ls / 16.0f;

		g[i].cr = 1.0f;
		g[i].cg = 1.0f;
		g[i].cb = 1.0f;

		Vector3f normal = DirectionVector[i > 4 ? SOUTH : NORTH];

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
void BuildChunkModel( CModel &mdl, CModel &wmdl, CBlock blocks[], Vector3f pos, CChunk *chunk )
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
					// TODO: Get Model from Block
					for ( int i = 0; i < 6; i++ )
					{
						CColour lightColour;
						CColour blockColouration =
							BlockType( block.m_iBlockType )
								.GetTint( chunk, pos, block.m_iBlockData, (Direction)i );

						Vector3f neighbour = Vector3f( x, y, z ) + DirectionVector[i];
						if ( ValidChunkPosition( neighbour ) )
						{
							BLOCKID blockType = chunk->GetBlockAtLocal( neighbour )->m_iBlockType;

							if ( !BlockType( block.m_iBlockType ).FaceVisible( (Direction)i, blockType ) )
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

								CBlock *b = chunkNeighbour->GetBlockAtLocal( neighbour );
								if ( b == nullptr )
									continue;
								if ( !BlockType( block.m_iBlockType ).FaceVisible( (Direction)i, b->m_iBlockType ) )
									continue;

								lightColour = chunkNeighbour->GetLightingLocal( neighbour );
							}
						}

						std::vector<CModel::Vertex> g =
							sampleCubeFace( Direction( i ), block, x, y, z, lightColour, blockColouration );

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
				}
			}
		}
	}

	mdl.Update();
	wmdl.Update();
}