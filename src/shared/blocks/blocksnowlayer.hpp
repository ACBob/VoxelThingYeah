#include "blockbase.hpp"

class CBlockSnowLayer : public CBlockBase
{
	using Super = CBlockBase;

  public:
	using Super::Super;

    virtual bool IsFullCube() const {
        return false;
    }

    virtual bool IsSolid( BLOCKVAL metadata ) const {
        return false;
    }

    virtual bool IsTransparent() const {
        return true;
    }

    virtual CBoundingBox GetBounds() const {
        return CBoundingBox( Vector3f( 0 ), Vector3f( BLOCKUNIT, (2.0f / 16.0f) * BLOCKUNIT, BLOCKUNIT ), Vector3f( 0 ) );
    }
};