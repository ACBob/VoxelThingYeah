#include "jeneration.hpp"

#include "world/world.hpp"

#include <math.h>

#include "structure.hpp"

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
	m_biomesOvergroundTemperatureNoise.octaves	  = 1;
	m_biomesOvergroundTemperatureNoise.frequency  = 0.02f;

	m_biomesOvergroundHumidityNoise				 = fnlCreateState();
	m_biomesOvergroundHumidityNoise.seed		 = m_iSeed + 22106;
	m_biomesOvergroundHumidityNoise.octaves		 = 1;
	m_biomesOvergroundHumidityNoise.noise_type	 = FNL_NOISE_PERLIN;
	m_biomesOvergroundTemperatureNoise.frequency = 0.01f;

	m_treeNoise									 = fnlCreateState();
	m_treeNoise.seed							 = m_iSeed + 'TREE';
	m_treeNoise.noise_type						 = FNL_NOISE_PERLIN;
	m_treeNoise.octaves							 = 1;
	m_treeNoise.frequency						 = 0.01f;

	// Biomes
	InitBiomes();
}

// Generates the base stone skeleton
void COverworldJeneration::GenBase( CChunk *c )
{
	for ( int i = 0; i < sizeof( c->m_blocks ) / sizeof( block_t ); i++ )
	{
		// blocks[i].m_iBlockType = BLOCKID(random() % 4);
		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );
		Vector3f WorldPosition = c->PosToWorld( Vector3f( x, y, z ) );

		// Make the block aware of our existence
		c->m_blocks[i].m_pChunk = c;

		// First try the seafloor
		float noiseDataFloor = fnlGetNoise2D( &m_seafloorNoise, WorldPosition.x, WorldPosition.z );
		float seaFloor		 = SEA_FLOOR + ( 3.0f * noiseDataFloor );

		if ( WorldPosition.y <= seaFloor )
		{
			c->m_blocks[i].Set(STONE);
			continue;
		}

		float noiseData3D = 1 + fnlGetNoise3D( &m_baseNoise, WorldPosition.x, WorldPosition.y, WorldPosition.z );
		float percentToTopSurface = 1.0f - ( WorldPosition.y / 32.0f );
		noiseData3D *= percentToTopSurface;

		c->m_blocks[i].Set(noiseData3D > 0.7 ? STONE : ( WorldPosition.y > m_iSeaLevel ? AIR : WATERSRC ));
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
				block_t *blk = c->GetBlockAtLocal( Vector3f( x, y, z ) );
				if ( blk != nullptr && blk->GetType() == AIR )
					continue;

				block_t *b = c->GetBlockAtLocal( Vector3f( x, y + 1, z ) );
				if ( b == nullptr )
					continue;

				CBiome *biome = GetBiomeAtPos( c->PosToWorld( Vector3f( x, y, z ) ) );

				if ( b->GetType() == AIR )
				{
					if ( blk->GetType() == STONE )
					{
						b->Set( biome->m_iBlockDust );
						blk->Set( biome->m_iBlockSurface );
						grassDepth--;
					}
					else
					{
						blk->Set( biome->m_iBlockWaterSurf );
					}
				}
				else if ( b->GetType() == biome->m_iBlockSurface ||
						  b->GetType() == biome->m_iBlockSubSurface && grassDepth > 0 )
				{
					blk->Set( biome->m_iBlockSubSurface );
					grassDepth--;
				}
				else if ( blk->GetType() == STONE )
				{
					blk->Set( biome->m_iBlockRock );
				}
			}
		}
	}
}

// Decorates with ores, plants, etc.
void COverworldJeneration::Decorate( CChunk *c )
{
	for ( int i = 0; i < sizeof( c->m_blocks ) / sizeof( block_t ); i++ )
	{
		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );
		Vector3f WorldPosition = c->PosToWorld( Vector3f( x, y, z ) );

		// Ore
		if ( c->m_blocks[i].GetType() == STONE )
		{
			float noiseData = fnlGetNoise3D( &m_oreNoise, WorldPosition.x, WorldPosition.y, WorldPosition.z ) * 1.1;

			if ( noiseData > 0.9 )
				c->m_blocks[i].Set( ORE_COAL );
		}

		// Caves
		float caveVal = 0.0f;
		for ( int j = 0; j < CAVE_NOISES; j++ )
			caveVal +=
				std::pow( fnlGetNoise3D( &m_caveNoises[j], WorldPosition.x, WorldPosition.y, WorldPosition.z ), 2.0f );

		if ( caveVal < 0.04f )
		{
			if ( c->m_blocks[i].GetType() == STONE || c->m_blocks[i].GetType() == GRASS ||
				 c->m_blocks[i].GetType() == DIRT || c->m_blocks[i].GetType() == ORE_COAL ||
				 c->m_blocks[i].GetType() == SAND || c->m_blocks[i].GetType() == SANDSTONE )
			{
				c->m_blocks[i].Set( AIR );
			}
		}
	}

	// As a last step, structures
	CStructure tree = GetStructure(TREE);

	// Test if we're in a chunk that has a tree
	if ( true )
	{
		// Find a place to put the tree
		int x = 0, z = 0;

		x = rand() % CHUNKSIZE_X;
		z = rand() % CHUNKSIZE_Z;

		// Make sure we're not in a cave ( so above sea level )
		if ( c->GetPosInWorld({(float)x,0,(float)z}).y > SEA_LEVEL )
		{
			// find the surface block
			int y = 0;
			for ( y = CHUNKSIZE_Y - 1; y > 0; y-- )
			{
				if ( c->GetBlockAtLocal({(float)x, (float)y, (float)z})->GetType() != AIR )
					break;
			}

			// Don't place on air
			if ( y <= 0 )
				return;

			// Place the tree
			// world coords
			Vector3f worldPosition = c->PosToWorld({(float)x, (float)y, (float)z});
			tree.Generate( c->m_pChunkMan, worldPosition.x, worldPosition.y, worldPosition.z, (Direction)(rand() % 4) );
		}
	}
}

void COverworldJeneration::Generate( CChunk *c )
{
	GenBase( c );
	BiomeBlocks( c );
	Decorate( c );
}

CBiome *COverworldJeneration::GetBiomeAtPos( Vector3f p )
{
	// temperature & humidity are 0 - 2
	float fTemperature = ( fnlGetNoise2D( &m_biomesOvergroundTemperatureNoise, p.x, p.z ) + 1.0f );
	float fHumidity	   = ( fnlGetNoise2D( &m_biomesOvergroundHumidityNoise, p.x, p.z ) + 1.0f );

	// return the biome that is closest to the given temperature and humidity
	float fClosestDistance = 99999.0f;
	CBiome *pClosestBiome  = nullptr;
	for ( int i = 0; i < biomes::biomeList.size(); i++ )
	{
		float fDistance = std::abs( biomes::biomeList[i]->m_fTemperature - fTemperature ) +
						  std::abs( biomes::biomeList[i]->m_fHumidity - fHumidity );
		if ( fDistance < fClosestDistance )
		{
			fClosestDistance = fDistance;
			pClosestBiome	 = biomes::biomeList[i];
		}
	}

	return pClosestBiome;
}