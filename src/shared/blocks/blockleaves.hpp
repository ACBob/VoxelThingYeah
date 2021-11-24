#include "blocktransgeneric.hpp"

class CBlockLeaves : public CBlockTransGeneric
{
    using Super = CBlockTransGeneric;

  public:
    using Super::Super;

#ifdef CLIENTEXE
    virtual bool FaceVisible( Direction direction, BLOCKID blockId ) const
    {
        return BlockType( blockId ).IsTransparent() || BlockType( blockId ).IsLiquid() || !BlockType( blockId ).IsFullCube();
    }
#endif
};