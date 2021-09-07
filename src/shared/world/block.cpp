#include "block.hpp"
#include "chunk.hpp"

#include <stdio.h>

CBlock::CBlock() : chunk( nullptr ) {}

void CBlock::Update()
{
	if ( chunk != nullptr )
		reinterpret_cast<CChunk *>( chunk )->Update();
}

#ifdef CLIENTEXE
BlockTexture CBlock::GetSideTexture( Direction side ) { return GetDefaultBlockTextureSide( this->blockType, side ); }
#endif