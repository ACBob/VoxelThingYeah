#include "block.hpp"
#include "chunk.hpp"
#include "fastnoise.h"

#pragma once

class COverworldJeneration
{
  public:
	COverworldJeneration();

	void Generate( CChunk *c );

	void GenBase( CChunk *c );
	void BiomeBlocks( CChunk *c );
	void Decorate( CChunk *c );

  private:
	fnl_state m_baseNoise;
	fnl_state m_seafloorNoise;
	fnl_state m_dirtNoise;
	fnl_state m_oreNoise;

	int m_iSeed = 69;

	int m_iSeaLevel = 10;
};