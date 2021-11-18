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

	biomePlains.m_iBlockSurface	   = GRASS;
	biomePlains.m_iBlockSubSurface = DIRT;
	biomePlains.m_iBlockRock	   = STONE;
	biomePlains.m_fTemperature	   = 1.1f;
	biomePlains.m_fHumidity		   = 0.4f;
	biomeList.push_back( &biomePlains );

	biomeDesert.m_iBlockSurface	   = SAND;
	biomeDesert.m_iBlockSubSurface = SAND;
	biomeDesert.m_iBlockRock	   = SANDSTONE;
	biomeDesert.m_fTemperature	   = 2.0f;
	biomeDesert.m_fHumidity		   = 0.0f;
	biomeList.push_back( &biomeDesert );

	biomeTundra.m_iBlockSurface	   = SNOWGRASS;
	biomeTundra.m_iBlockSubSurface = DIRT;
	biomeTundra.m_iBlockRock	   = STONE;
	biomeTundra.m_iBlockDust	   = SNOW;
	biomeTundra.m_iBlockWaterSurf  = ICE;
	biomeTundra.m_fTemperature	   = 0.0f;
	biomeTundra.m_fHumidity		   = 0.5f;
	biomeList.push_back( &biomeTundra );
}