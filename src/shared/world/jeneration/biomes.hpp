#include "blockdef.hpp"

#include <vector>

class CBiome
{
  public:
	CBiome();
	~CBiome();

	// These control blocks used
	blocktype_t m_iBlockSurface	   = AIR; // GRASS
	blocktype_t m_iBlockSubSurface = AIR; // DIRT
	blocktype_t m_iBlockRock	   = AIR; // STONE

	// 0 To 2, controls generation
	float m_fTemperature = 0.0f;
	float m_fHumidity	 = 0.0f;
};

void InitBiomes(); // Initialises the biomes

namespace biomes
{
	extern CBiome biomePlains;
	extern CBiome biomeDesert;

	extern std::vector<CBiome *> biomeList;
} // namespace biomes