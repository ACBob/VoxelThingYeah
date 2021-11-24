#include "blockbase.hpp"

class CBlockPlant : public CBlockBase
{
    using Super = CBlockBase;

    public:
        using Super::Super;

        virtual bool IsSolid( BLOCKVAL metadata ) const { return false; }
        virtual bool IsTransparent() const { return true; }
        virtual bool IsFullCube() const { return false; }

#ifdef CLIENTEXE        
        virtual bool FaceVisible( Direction direction, BLOCKID blockId ) const
        {
            return true;
        }

        virtual BLOCKMODEL GetModel( CChunk *pChunk, Vector3f pos, BLOCKVAL metadata ) const
        {
            return BLOCKMODEL_CROSS;
        }
#endif
};