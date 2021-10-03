#include "blockdef.hpp"

#include <vector>

class CBiome
{
  public:
	CBiome();
	~CBiome();

	// These control blocks used
	blocktype_t m_iBlockSurface	   = AIR;	   // GRASS
	blocktype_t m_iBlockSubSurface = AIR;	   // DIRT
	blocktype_t m_iBlockRock	   = AIR;	   // STONE
	blocktype_t m_iBlockDust	   = AIR;	   // E.G SNOW
	blocktype_t m_iBlockWaterSurf  = WATERSRC; // E.G ICE

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