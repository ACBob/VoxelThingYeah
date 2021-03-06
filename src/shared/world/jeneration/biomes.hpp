#include "blockdef.hpp"

#include <vector>

class CBiome
{
  public:
	CBiome();
	~CBiome();

	// These control blocks used
	BLOCKID m_iBlockSurface	   = AIR;	   // GRASS
	BLOCKID m_iBlockSubSurface = AIR;	   // DIRT
	BLOCKID m_iBlockRock	   = AIR;	   // STONE
	BLOCKID m_iBlockDust	   = AIR;	   // E.G SNOW
	BLOCKID m_iBlockWaterSurf  = WATERSRC; // E.G ICE

	// 0 To 2, controls generation
	float m_fTemperature = 0.0f;
	float m_fHumidity	 = 0.0f;
};

void InitBiomes(); // Initialises the biomes

namespace biomes
{
	extern CBiome biomePlains;
	extern CBiome biomeDesert;
	extern CBiome biomeTundra;

	extern std::vector<CBiome *> biomeList;
} // namespace biomes