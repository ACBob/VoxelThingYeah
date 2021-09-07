#include "jeneration.hpp"

#include "world.hpp"

#include <math.h>

COverworldJeneration::COverworldJeneration()
{
	m_baseNoise			 = fnlCreateState();
	m_baseNoise.seed		 = m_iSeed;
	m_baseNoise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	m_baseNoise.frequency	 = 0.025;

	m_dirtNoise			 = fnlCreateState();
	m_dirtNoise.seed		 = m_iSeed + 230;
	m_dirtNoise.noise_type = FNL_NOISE_PERLIN;
	m_dirtNoise.frequency	 = 0.0125;

	m_oreNoise				   = fnlCreateState();
	m_oreNoise.seed			   = m_iSeed - 2587;
	m_oreNoise.noise_type		   = FNL_NOISE_OPENSIMPLEX2;
	m_oreNoise.frequency		   = 0.075;
	m_oreNoise.octaves		   = 4;
	m_oreNoise.lacunarity		   = 0.6;
	m_oreNoise.gain			   = 1.3;
	m_oreNoise.weighted_strength = 0.8;
}

// Generates the base stone skeleton
void COverworldJeneration::GenBase( CChunk *c )
{
	for ( int i = 0; i < sizeof( c->m_blocks ) / sizeof( CBlock ); i++ )
	{
		// blocks[i].m_iBlockType = blocktype_t(random() % 4);
		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );
		CVector WorldPosition = c->PosToWorld( CVector( x, y, z ) );

		// Make the block aware of our existence
		c->m_blocks[i].m_pChunk = c;

		float noiseData			  = 1 + fnlGetNoise3D( &m_baseNoise, WorldPosition.x, WorldPosition.y, WorldPosition.z );
		float percentToTopSurface = 1.0f - ( WorldPosition.y / 32.0f );
		noiseData *= percentToTopSurface;

		c->m_blocks[i].m_iBlockType = noiseData > 0.7 ? blocktype_t::STONE : blocktype_t::AIR;
	}
}

// Replaces the surface and sometimes under that with block suiteable for the biome
void COverworldJeneration::BiomeBlocks( CChunk *c )
{
	// Grassification
	for ( int x = 0; x < CHUNKSIZE_X; x++ )
	{
		for ( int z = 0; z < CHUNKSIZE_Z; z++ )
		{
			int grassDepth =
				2 + ( 2 * ( 1 + fnlGetNoise2D( &m_dirtNoise, x + c->GetPosInWorld().x, z + c->GetPosInWorld().z ) ) );

			// We follow down to -1 so we can alter the blocks in the chunk below
			for ( int y = CHUNKSIZE_Y; y > -1; y-- )
			{
				CBlock *blk = c->GetBlockAtLocal( CVector( x, y, z ) );
				if ( blk != nullptr && blk->m_iBlockType == AIR )
					continue;

				CBlock *b = c->GetBlockAtLocal( CVector( x, y + 1, z ) );
				if ( b == nullptr )
					continue;

				if ( b->m_iBlockType == AIR )
				{
					blk->m_iBlockType = GRASS;
					grassDepth--;
				}
				else if ( b->m_iBlockType == GRASS || b->m_iBlockType == DIRT && grassDepth > 0 )
				{
					blk->m_iBlockType = DIRT;
					grassDepth--;
				}
			}
		}
	}
}

// Decorates with ores, plants, etc.
void COverworldJeneration::Decorate( CChunk *c )
{
	// Ore
	for ( int i = 0; i < sizeof( c->m_blocks ) / sizeof( CBlock ); i++ )
	{
		if ( c->m_blocks[i].m_iBlockType != STONE )
			continue;

		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );
		CVector WorldPosition = c->PosToWorld( CVector( x, y, z ) );

		float noiseData = fnlGetNoise3D( &m_oreNoise, WorldPosition.x, WorldPosition.y, WorldPosition.z ) * 1.1;

		if ( noiseData > 0.9 )
			c->m_blocks[i].m_iBlockType = ORE_COAL;
	}
}

void COverworldJeneration::Generate( CChunk *c )
{
	GenBase( c );
	BiomeBlocks( c );
	Decorate( c );
}