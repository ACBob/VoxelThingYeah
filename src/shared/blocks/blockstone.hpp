#pragma once

#include "blockbase.hpp"

class CBlockStone final : public CBlockBase
{
    using Super = CBlockBase;
    public:
    
    using Super::Super;

    virtual BLOCKMATERIAL GetMaterial( ) const {
		return MAT_STONE;
	};
};