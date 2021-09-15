#include "jeneration.hpp"

#include "world/world.hpp"

#include <math.h>

COverworldJeneration::COverworldJeneration()
{
	m_baseNoise			   = fnlCreateState();
	m_baseNoise.seed	   = m_iSeed;
	m_baseNoise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	m_baseNoise.frequency  = 0.005;

	m_seafloorNoise			   = fnlCreateState();
	m_seafloorNoise.seed	   = m_iSeed + 23;
	m_seafloorNoise.noise_type = FNL_NOISE_PERLIN;
	m_seafloorNoise.octaves	   = 1;
	m_seafloorNoise.lacunarity = 0.3;

	m_dirtNoise			   = fnlCreateState();
	m_dirtNoise.seed	   = m_iSeed + 230;
	m_dirtNoise.noise_type = FNL_NOISE_PERLIN;
	m_dirtNoise.frequency  = 0.0125;

	m_oreNoise					 = fnlCreateState();
	m_oreNoise.seed				 = m_iSeed - 2587;
	m_oreNoise.noise_type		 = FNL_NOISE_OPENSIMPLEX2;
	m_oreNoise.frequency		 = 0.075;
	m_oreNoise.octaves			 = 4;
	m_oreNoise.lacunarity		 = 0.6;
	m_oreNoise.gain				 = 1.3;
	m_oreNoise.weighted_strength = 0.8;

	for ( int i = 0; i < CAVE_NOISES; i++ )
	{
		m_caveNoises[i]			  = fnlCreateState();
		m_caveNoises[i].seed	  = m_iSeed + 'CAVE' + i;
		m_caveNoises[i].frequency = 0.01;
	}

	// Biome numbers provided by Jen
	m_biomesOvergroundTemperatureNoise			  = fnlCreateState();
	m_biomesOvergroundTemperatureNoise.seed		  = m_iSeed + 102;
	m_biomesOvergroundTemperatureNoise.noise_type = FNL_NOISE_PERLIN;
	m_biomesOvergroundTemperatureNoise.octaves    = 1;
	m_biomesOvergroundTemperatureNoise.frequency  = 0.01f;

	m_biomesOvergroundHumidityNoise				 = fnlCreateState();
	m_biomesOvergroundHumidityNoise.seed		 = m_iSeed + 22106;
	m_biomesOvergroundHumidityNoise.octaves      = 1;
	m_biomesOvergroundHumidityNoise.noise_type	 = FNL_NOISE_PERLIN;
	m_biomesOvergroundTemperatureNoise.frequency = 0.01f;

	// Biomes
	InitBiomes();
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

		// First try the seafloor
		float noiseDataFloor = fnlGetNoise2D( &m_seafloorNoise, WorldPosition.x, WorldPosition.z );
		float seaFloor		 = SEA_FLOOR + ( 3.0f * noiseDataFloor );

		if ( WorldPosition.y <= seaFloor )
		{
			c->m_blocks[i].m_iBlockType = STONE;
			continue;
		}

		float noiseData3D = 1 + fnlGetNoise3D( &m_baseNoise, WorldPosition.x, WorldPosition.y, WorldPosition.z );
		float percentToTopSurface = 1.0f - ( WorldPosition.y / 32.0f );
		noiseData3D *= percentToTopSurface;

		c->m_blocks[i].m_iBlockType = noiseData3D > 0.7 ? STONE : ( WorldPosition.y > m_iSeaLevel ? AIR : WATER );
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
				// TODO: Actually fix this as it tries to query chunks that don't exist yet.
				CBlock *blk = c->GetBlockAtLocal( CVector( x, y, z ) );
				if ( blk != nullptr && blk->m_iBlockType != STONE )
					continue;

				CBlock *b = c->GetBlockAtLocal( CVector( x, y + 1, z ) );
				if ( b == nullptr )
					continue;

				CBiome *biome = GetBiomeAtPos( c->PosToWorld( CVector( x, y, z ) ) );

				if ( b->m_iBlockType == AIR )
				{
					blk->m_iBlockType = biome->m_iBlockSurface;
					grassDepth--;
				}
				else if ( b->m_iBlockType == biome->m_iBlockSurface ||
						  b->m_iBlockType == biome->m_iBlockSubSurface && grassDepth > 0 )
				{
					blk->m_iBlockType = biome->m_iBlockSubSurface;
					grassDepth--;
				}
				else if ( blk->m_iBlockType == STONE )
				{
					blk->m_iBlockType = biome->m_iBlockRock;
				}
			}
		}
	}
}

// Decorates with ores, plants, etc.
void COverworldJeneration::Decorate( CChunk *c )
{
	for ( int i = 0; i < sizeof( c->m_blocks ) / sizeof( CBlock ); i++ )
	{
		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );
		CVector WorldPosition = c->PosToWorld( CVector( x, y, z ) );

		// Ore
		if ( c->m_blocks[i].m_iBlockType == STONE )
		{
			float noiseData = fnlGetNoise3D( &m_oreNoise, WorldPosition.x, WorldPosition.y, WorldPosition.z ) * 1.1;

			if ( noiseData > 0.9 )
				c->m_blocks[i].m_iBlockType = ORE_COAL;
		}

		// Caves
		float caveVal = 0.0f;
		for ( int j = 0; j < CAVE_NOISES; j++ )
			caveVal +=
				std::pow( fnlGetNoise3D( &m_caveNoises[j], WorldPosition.x, WorldPosition.y, WorldPosition.z ), 2.0f );

		if ( caveVal < 0.04f )
		{
			if ( c->m_blocks[i].m_iBlockType == STONE || c->m_blocks[i].m_iBlockType == GRASS ||
				 c->m_blocks[i].m_iBlockType == DIRT || c->m_blocks[i].m_iBlockType == ORE_COAL ||
				 c->m_blocks[i].m_iBlockType == SAND || c->m_blocks[i].m_iBlockType == SANDSTONE )
			{
				c->m_blocks[i].m_iBlockType = AIR;
			}
		}
	}
}

void COverworldJeneration::Generate( CChunk *c )
{
	GenBase( c );
	BiomeBlocks( c );
	Decorate( c );
}

CBiome *COverworldJeneration::GetBiomeAtPos( CVector p )
{
	float fTemperature = 1.35f + fnlGetNoise2D( &m_biomesOvergroundTemperatureNoise, p.x, p.z );
	// float fHumidity = fnlGetNoise2D( &m_biomesOvergroundHumidityNoise, p.x, p.z );

	CBiome *closestBiome = biomes::biomeList[0];

	for ( CBiome *biome : biomes::biomeList )
	{
		if ( fabsf( biome->m_fTemperature - fTemperature ) < fabsf( closestBiome->m_fTemperature - fTemperature ) )
			closestBiome = biome;
	}

	return closestBiome;
}