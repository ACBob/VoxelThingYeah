#include "block.hpp"
#include "chunk.hpp"

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

#ifdef CLIENTEXE
BlockTexture Block::GetSideTexture(Direction side)
{
	return GetDefaultBlockTextureSide(this->blockType, side);
}
#endif