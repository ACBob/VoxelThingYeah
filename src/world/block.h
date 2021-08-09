#include "utility/direction.h"
#include "utility/vector.h"
#include "blockdef.h"

#include "rendering/blocktexture.h"

#pragma once

class Block {
	public:
		Block();

		blocktype_t blockType = blocktype_t::AIR;

		BlockTexture GetSideTexture(Direction side);

		// Pos is in world coords
		bool TestPointCollision(Vector pos);

		Vector worldPos;

		// block update
		// causes a mesh rebuild
		void Update();

		// Pointer to chunk that holds us
		void* chunk;
};