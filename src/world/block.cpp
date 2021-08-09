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

bool Block::TestAABBCollision(Vector pos, Vector size)
{
	if (blockType == blocktype_t::AIR)
		return false;

	Vector minA = pos;
	Vector maxA = minA + size;
	Vector minB = worldPos;
	Vector maxB = minB + Vector(BLOCKUNIT, BLOCKUNIT, BLOCKUNIT);
	if (
		(minA.x <= maxB.x && maxA.x >= minB.x) &&
		(minA.y <= maxB.y && maxA.y >= minB.y) &&
		(minA.z <= maxB.z && maxA.z >= minB.z)
	)
		return true;
	
	return false;
}

BlockTexture Block::GetSideTexture(Direction side)
{
	return GetDefaultBlockTextureSide(this->blockType, side);
}