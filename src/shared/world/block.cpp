#include "block.hpp"
#include "chunk.hpp"

#include <stdio.h>

void block_t::Update()
{
	if ( m_pChunk != nullptr )
		reinterpret_cast<CChunk *>( m_pChunk )->m_bDirty = true; // mark parent chunk as dirty
}

#ifdef CLIENTEXE
BlockTexture block_t::GetSideTexture( Direction side ) { return GetDefaultBlockTextureSide( this->GetType(), side ); }
#endif