#include "blockbase.hpp"

class CBlockPlant : public CBlockBase
{
    using Super = CBlockBase;

    public:
        using Super::Super;

        virtual bool IsSolid( BLOCKVAL metadata ) const { return false; }
        virtual bool IsTransparent() const { return true; }
        virtual bool IsFullCube() const { return false; }
};