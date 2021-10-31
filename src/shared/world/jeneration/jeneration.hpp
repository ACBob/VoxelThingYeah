#include "biomes.hpp"
#include "fastnoise.h"
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

	CBiome *GetBiomeAtPos( CVector p );

  private:
	fnl_state m_baseNoise;
	fnl_state m_seafloorNoise;
	fnl_state m_dirtNoise;
	fnl_state m_oreNoise;

	fnl_state m_caveNoises[CAVE_NOISES];

	fnl_state m_biomesOvergroundTemperatureNoise;
	fnl_state m_biomesOvergroundHumidityNoise;
	fnl_state m_biomesUndergroundNoise;

	int m_iSeed = 69;

	int m_iSeaLevel = 10;
};