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
	m_baseNoise.octaves	   = 5;

	m_hillNoise			   = fnlCreateState();
	m_hillNoise.seed	   = m_iSeed + 'H';
	m_hillNoise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	m_hillNoise.frequency  = 0.01;
	m_hillNoise.octaves	   = 1;

	m_riverNoise		   = fnlCreateState();
	m_riverNoise.seed	   = m_iSeed + 'R';
	m_riverNoise.noise_type = FNL_NOISE_CELLULAR;
	m_riverNoise.frequency  = 0.01;
	m_riverNoise.octaves	   = 4;
	m_riverNoise.cellular_distance_func = FNL_CELLULAR_DISTANCE_EUCLIDEANSQ;

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
		m_caveNoises[i].frequency = 0.01 * ( i + 1 );
		m_caveNoises[i].octaves	  = 1 + i;
		m_caveNoises[i].noise_type = FNL_NOISE_PERLIN;
	}

	// Biome numbers provided by Jen
	m_biomesOvergroundTemperatureNoise			  = fnlCreateState();
	m_biomesOvergroundTemperatureNoise.seed		  = m_iSeed + 102;
	m_biomesOvergroundTemperatureNoise.noise_type = FNL_NOISE_VALUE;
	m_biomesOvergroundTemperatureNoise.frequency  = 0.02f;
	m_biomesOvergroundTemperatureNoise.domain_warp_type = FNL_DOMAIN_WARP_BASICGRID;
	m_biomesOvergroundTemperatureNoise.fractal_type = FNL_FRACTAL_DOMAIN_WARP_PROGRESSIVE;
	m_biomesOvergroundTemperatureNoise.domain_warp_amp = 50.0f;
	m_biomesOvergroundTemperatureNoise.frequency = 0.01f;
	m_biomesOvergroundTemperatureNoise.octaves = 5;

	m_biomesOvergroundHumidityNoise				 = fnlCreateState();
	m_biomesOvergroundHumidityNoise.seed		 = m_iSeed + 22106;
	m_biomesOvergroundTemperatureNoise.domain_warp_type = FNL_DOMAIN_WARP_BASICGRID;
	m_biomesOvergroundTemperatureNoise.fractal_type = FNL_FRACTAL_DOMAIN_WARP_PROGRESSIVE;
	m_biomesOvergroundTemperatureNoise.domain_warp_amp = 50.0f;
	m_biomesOvergroundTemperatureNoise.frequency = 0.02f;
	m_biomesOvergroundTemperatureNoise.octaves = 5;

	m_treeNoise			   = fnlCreateState();
	m_treeNoise.seed	   = m_iSeed + 'TREE';
	m_treeNoise.noise_type = FNL_NOISE_PERLIN;
	m_treeNoise.octaves	   = 1;
	m_treeNoise.frequency  = 0.01f;

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
			c->m_blocks[i].SetType( STONE );
			continue;
		}

		// base noise
		float noiseData3D = 1 + fnlGetNoise3D( &m_baseNoise, WorldPosition.x, WorldPosition.y, WorldPosition.z );
		// Hill noise
		noiseData3D += fmaxf(fnlGetNoise2D( &m_hillNoise, WorldPosition.x, WorldPosition.z ), 0);
		

		float percentToTopSurface = 1.0f - ( WorldPosition.y / 64.0f );
		noiseData3D *= percentToTopSurface;

		// River noise
		noiseData3D += fminf(fnlGetNoise2D( &m_riverNoise, WorldPosition.x, WorldPosition.z ), 0);

		c->m_blocks[i].SetType( noiseData3D > 0.7 ? STONE : ( WorldPosition.y > m_iSeaLevel ? AIR : WATERSRC ) );
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
				if ( c->PosToWorld( Vector3f( x, y, z ) ).y < m_iSeaLevel )
					continue;

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
						b->SetType( biome->m_iBlockDust );
						blk->SetType( biome->m_iBlockSurface );
						grassDepth--;
					}
					else
					{
						blk->SetType( biome->m_iBlockWaterSurf );
					}
				}
				else if ( b->GetType() == biome->m_iBlockSurface ||
						  b->GetType() == biome->m_iBlockSubSurface && grassDepth > 0 )
				{
					blk->SetType( biome->m_iBlockSubSurface );
					grassDepth--;
				}
				else if ( blk->GetType() == STONE )
				{
					blk->SetType( biome->m_iBlockRock );
				}
			}
		}
	}

	// Now for everything below the sea, set the surface to gravel
	for ( int x = 0; x < CHUNKSIZE_X; x++ )
	{
		for ( int z = 0; z < CHUNKSIZE_Z; z++ )
		{
			int gravelDepth =
				2 + ( 2 * ( 1 + fnlGetNoise2D( &m_dirtNoise, x + c->GetPosInWorld().x, z + c->GetPosInWorld().z ) ) );
			for ( int y = 0; y < CHUNKSIZE_Y; y++ )
			{
				if ( c->PosToWorld( Vector3f( x, y, z ) ).y >= m_iSeaLevel )
					continue;

				block_t *blk = c->GetBlockAtLocal( Vector3f( x, y, z ) );
				if ( blk == nullptr )
					continue;
				
				block_t *b = c->GetBlockAtLocal( Vector3f( x, y + 1, z ) );
				if ( b == nullptr )
					continue;

				if ( (b->GetType() == WATER || b->GetType() == WATERSRC) && blk->GetType() == STONE )
				{
					blk->SetType( GRAVEL );
					gravelDepth--;
				}
				else if ( b->GetType() == GRAVEL && gravelDepth > 0 && blk->GetType() == STONE )
				{
					blk->SetType( GRAVEL );
					gravelDepth--;
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
				c->m_blocks[i].SetType( ORE_COAL );
		}

		// Caves
		float caveVal = 0.0f;
		for ( int j = 0; j < CAVE_NOISES; j++ )
			caveVal +=
				std::pow( fnlGetNoise3D( &m_caveNoises[j], WorldPosition.x, WorldPosition.y, WorldPosition.z ), 2.0f );

		if ( caveVal < 0.04f )
		{
			BLOCKID id = c->m_blocks[i].GetType();
			if ( id == STONE || id == GRASS     ||
				 id == DIRT  || id == ORE_COAL  ||
				 id == SAND  || id == SANDSTONE ||
				 id == GRAVEL|| id == SNOWGRASS ||
				 id == SNOWLAYER )
			{
				c->m_blocks[i].SetType( AIR );
			}
		}
	}

	// As a last step, structures
	CStructure tree = GetStructure( TREE );

	// Test if we're in a chunk that has a tree
	if ( true )
	{
		// Find a place to put the tree
		int x = 0, z = 0;

		x = rand() % CHUNKSIZE_X;
		z = rand() % CHUNKSIZE_Z;

		// Make sure we're not in a cave ( so above sea level )
		if ( c->GetPosInWorld( { (float)x, 0, (float)z } ).y > SEA_LEVEL )
		{
			// find the surface block
			int y = 0;
			for ( y = CHUNKSIZE_Y - 1; y > 0; y-- )
			{
				if ( c->GetBlockAtLocal( { (float)x, (float)y, (float)z } )->GetType() == GRASS )
					break;
			}

			// Don't place on air
			if ( y <= 0 )
				return;

			y ++; // Move out of the block so that we don't get stuck in the ground

			// Place the tree
			// world coords
			Vector3f worldPosition = c->PosToWorld( { (float)x, (float)y, (float)z } );
			tree.Generate( c->m_pChunkMan, worldPosition.x, worldPosition.y, worldPosition.z,
						   ( Direction )( rand() % 4 ) );
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