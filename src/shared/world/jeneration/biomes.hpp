#include "blockdef.hpp"

#include <vector>

class CBiome
{
  public:
	CBiome();
	~CBiome();

	// These control blocks used
	BLOCKID m_iBlockSurface	   = BLCK_AIR;	   // BLCK_GRASS
	BLOCKID m_iBlockSubSurface = BLCK_AIR;	   // DIRT
	BLOCKID m_iBlockRock	   = BLCK_AIR;	   // STONE
	BLOCKID m_iBlockDust	   = BLCK_AIR;	   // E.G BLCK_SNOW
	BLOCKID m_iBlockWaterSurf  = BLCK_WATERSRC; // E.G BLCK_ICE

	// 0 To 2, controls generation
	float m_fMinTemperature = 0.0f;
	float m_fMaxTemperature = 0.0f;
	float m_fHumidity		= 0.0f;
};

void InitBiomes(); // Initialises the biomes

namespace biomes
{
	extern CBiome biomePlains;
	extern CBiome biomeDesert;
	extern CBiome biomeTundra;

	extern std::vector<CBiome *> biomeList;
} // namespace biomes