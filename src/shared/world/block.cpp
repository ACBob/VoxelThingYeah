#include "block.hpp"
#include "chunk.hpp"

#include <stdio.h>

CBlock::CBlock() : m_pChunk( nullptr ) {}

void CBlock::Update()
{
	if ( m_pChunk != nullptr )
		reinterpret_cast<CChunk *>( m_pChunk )->Update();
}

#ifdef CLIENTEXE
BlockTexture CBlock::GetSideTexture( Direction side ) { return GetDefaultBlockTextureSide( this->m_iBlockType, side ); }
#endif