#include "blockbase.hpp"

// Slabs are a bit... special.
// There's only one ID for slabs - They use their meta value to determine what they are.
// This is done because I'm not about to have a whole bunch of different block types.

class CBlockSlabGeneric : public CBlockBase
{
    using Super = CBlockBase;

    public:
    using Super::Super;

    virtual bool IsFullCube() const {
        return false;
    }

    virtual bool IsOpaque() const {
        return false;
    }

    virtual CBoundingBox GetBounds() const {
        return CBoundingBox( Vector3f( 0 ), Vector3f( BLOCKUNIT, BLOCKUNIT / 2, BLOCKUNIT ), Vector3f( 0 ) );
    }

    virtual BLOCKMATERIAL GetMaterial( BLOCKVAL meta ) const { return GetBlockMaterial( meta ); }; 
    
#ifdef CLIENTEXE
	virtual BlockTexture GetTexture( Direction direction, BLOCKVAL metadata ) const
	{
		return GetDefaultBlockTextureSide( metadata, direction );
	}

    virtual BLOCKMODEL GetModel( CChunk *pChunk, Vector3f pos, BLOCKVAL metadata ) const { return BLOCKMODEL_HALFBLOCK; };
#endif
};