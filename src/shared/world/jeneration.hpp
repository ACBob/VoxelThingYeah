#include "fastnoise.h"
#include "block.hpp"
#include "chunk.hpp"

#pragma once

class OverworldJeneration
{
	public:
		OverworldJeneration();
		void Generate(Chunk *c);
	
	private:
		fnl_state noise;

		int seed = 69;
};