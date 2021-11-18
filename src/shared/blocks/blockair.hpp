#pragma once

#include "blockbase.hpp"

class CBlockAir final : public CBlockBase
{
	using Super = CBlockBase;

  public:
	using Super::Super;

	virtual CBoundingBox GetBounds() const { return CBoundingBox( Vector3f( 0, 0, 0 ), Vector3f( 0, 0, 0 ) ); };

	virtual bool IsSolid( BLOCKVAL metadata ) const { return false; };
};