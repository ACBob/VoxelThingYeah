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

#ifdef CLIENTEXE
    virtual BLOCKMODEL GetModel( CChunk *pChunk, Vector3f pos, BLOCKVAL metadata ) const { return BLOCKMODEL_2LAYER; };
#endif
};