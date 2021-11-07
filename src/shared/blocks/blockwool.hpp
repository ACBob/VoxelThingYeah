#pragma once

#include "blockbase.hpp"

class CBlockWool final : public CBlockBase
{
    using Super = CBlockBase;
    public:
    
    using Super::Super;

#ifdef CLIENTEXE
	virtual uint16_t GetTint( CChunk *pChunk, CVector pos, BLOCKVAL metadata, Direction dir ) const {
		return metadata;
	};
#endif
};