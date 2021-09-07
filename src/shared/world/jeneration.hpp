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
	fnl_state baseNoise;
	fnl_state dirtNoise;
	fnl_state oreNoise;

	int seed = 69;
};