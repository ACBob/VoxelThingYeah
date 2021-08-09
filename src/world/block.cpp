#include "block.h"
#include "chunk.h"

#include <stdio.h>

Block::Block() :
	chunk(nullptr)
{

}

void Block::Update()
{
	if (chunk != nullptr)
		reinterpret_cast<Chunk*>(chunk)->Update();
}

bool Block::TestPointCollision(Vector pos)
{
	if (blockType == blocktype_t::AIR)
		return false;

	if (pos < worldPos || pos > worldPos + Vector(BLOCKUNIT, BLOCKUNIT, BLOCKUNIT))
		return false;
	
	return true;
}

BlockTexture Block::GetSideTexture(Direction side)
{
	return GetDefaultBlockTextureSide(this->blockType, side);
}