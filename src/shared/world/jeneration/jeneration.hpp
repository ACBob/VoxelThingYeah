#include "biomes.hpp"
#include "fastnoise.h"
#include "world/block.hpp"
#include "world/chunk.hpp"

#pragma once

#define CAVE_NOISES 3

#define SEA_LEVEL 10
#define SEA_FLOOR 8

class COverworldJeneration
{
  public:
	COverworldJeneration();

	void Generate( CChunk *c );

	void GenBase( CChunk *c );
	void BiomeBlocks( CChunk *c );
	void Decorate( CChunk *c );

	CBiome *GetBiomeAtPos( Vector3f p );

  private:
	fnl_state m_baseNoise; // Base noise, used for all biomes
	fnl_state m_hillNoise; // This is larger than the base noise, and is used for hills. Only added to the base noise.
	fnl_state m_riverNoise; // Same size as hills - and it's sharp. Only subtracts from the base noise.

	fnl_state m_seafloorNoise;
	fnl_state m_dirtNoise;
	fnl_state m_oreNoise;

	fnl_state m_caveNoises[CAVE_NOISES];

	fnl_state m_biomesOvergroundTemperatureNoise;
	fnl_state m_biomesOvergroundHumidityNoise;
	// fnl_state m_biomesUndergroundNoise;
	fnl_state m_treeNoise;

	uint32_t m_iSeed = 69;

	int m_iSeaLevel = 10;
};