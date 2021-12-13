#include "shared/blocks/blockbase.hpp"

class CClientBlockBase : public CBlockBase
{
    public:
        constexpr CClientBlockBase(BLOCKID blockType) : CBlockBase(blockType) {}
        
        virtual BlockTexture GetTexture( Direction direction, BLOCKVAL metadata ) const
        {
            return GetDefaultBlockTextureSide( m_iBlockType, direction );
        };
        virtual bool FaceVisible( Direction direction, BLOCKID blockId ) const;

        virtual uint16_t GetTint( CChunk *pChunk, Vector3f pos, BLOCKVAL metadata, Direction dir ) const { return 0xFFFF; };

        virtual BLOCKMODEL GetModel( CChunk *pChunk, Vector3f pos, BLOCKVAL metadata ) const { return BLOCKMODEL_CUBE; };
};