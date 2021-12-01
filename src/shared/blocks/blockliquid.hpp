#pragma once

#include "blockbase.hpp"

class CBlockLiquid final : public CBlockBase
{
	using Super = CBlockBase;

  public:
	using Super::Super;

	virtual bool IsSolid( BLOCKVAL metadata ) const { return false; };
	virtual bool IsSelectable( BLOCKVAL metadata ) const { return false; };
};