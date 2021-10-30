#include "biomes.hpp"

namespace biomes
{
	CBiome biomePlains;
	CBiome biomeDesert;
	CBiome biomeTundra;

	std::vector<CBiome *> biomeList;

	bool init = false;
} // namespace biomes

CBiome::CBiome(){};
CBiome::~CBiome(){};

using namespace biomes;
void InitBiomes()
{

	if ( !init )
		init = true;
	else
		return;

	biomePlains.m_iBlockSurface	   = BLCK_GRASS;
	biomePlains.m_iBlockSubSurface = BLCK_DIRT;
	biomePlains.m_iBlockRock	   = BLCK_STONE;
	biomePlains.m_fMinTemperature  = 0.6f;
	biomePlains.m_fMaxTemperature  = 1.6f;
	biomePlains.m_fHumidity		   = 0.4f;
	biomeList.push_back( &biomePlains );

	biomeDesert.m_iBlockSurface	   = BLCK_SAND;
	biomeDesert.m_iBlockSubSurface = BLCK_SAND;
	biomeDesert.m_iBlockRock	   = BLCK_SANDSTONE;
	biomeDesert.m_fMinTemperature  = 1.6f;
	biomeDesert.m_fMaxTemperature  = 2.0f;
	biomeDesert.m_fHumidity		   = 0.0f;
	biomeList.push_back( &biomeDesert );

	biomeTundra.m_iBlockSurface	   = BLCK_SNOWGRASS;
	biomeTundra.m_iBlockSubSurface = BLCK_DIRT;
	biomeTundra.m_iBlockRock	   = BLCK_STONE;
	biomeTundra.m_iBlockDust	   = BLCK_SNOW;
	biomeTundra.m_iBlockWaterSurf  = BLCK_ICE;
	biomeTundra.m_fMinTemperature  = 0.0f;
	biomeTundra.m_fMaxTemperature  = 0.6f;
	biomeTundra.m_fHumidity		   = 0.5f;
	biomeList.push_back( &biomeTundra );
}