#include "blockbase.hpp"

#pragma once

class CBlockTransGeneric : public CBlockBase
{
	using Super = CBlockBase;

  public:
	using Super::Super;

	virtual bool IsTransparent() const { return true; };
};