#include "block.hpp"
#include "chunk.hpp"

#include "blocks/blockbase.hpp"

#include <stdio.h>

void block_t::Update()
{
	if ( m_pChunk != nullptr )
		reinterpret_cast<CChunk *>( m_pChunk )->m_bDirty = true; // mark parent chunk as dirty
}