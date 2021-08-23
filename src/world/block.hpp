#include "utility/direction.hpp"
#include "utility/vector.hpp"
#include "blockdef.hpp"

#include "rendering/blocktexture.hpp"

#pragma once

class Block {
	public:
		Block();

		blocktype_t blockType = blocktype_t::AIR;

#ifdef CLIENTEXE
		BlockTexture GetSideTexture(Direction side);
#endif

		// Pos is in local coords
		bool TestPointCollision(Vector pos);
		// Pos is in local coords
		bool TestAABBCollision(Vector pos, Vector size);

		// block update
		// causes a mesh rebuild
		void Update();

		// Pointer to chunk that holds us
		void* chunk;
};