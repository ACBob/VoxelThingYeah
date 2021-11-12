#include "blockitem.hpp"
#include "blocks/blockbase.hpp"

CBlockItem::CBlockItem(int count, BLOCKID blockType, uint16_t blockData )
{ 
	m_iCount = count;
	m_iBlockType = blockType;
	m_iBlockData = blockData;

	m_iID = m_iBlockData;
};
CBlockItem::~CBlockItem(){};

int CBlockItem::GetCount()
{
	if ( m_iBlockType != AIR )
		return BaseClass::GetCount();
	return 0;
}

#ifdef CLIENTEXE
CTexture* CBlockItem::GetTexture()
{
	return materialSystem::LoadTexture( "terrain.png" );
}
Vector4f CBlockItem::GetUV() // TODO: This doesn't quite work (yet)
{
	Vector4f blockTex = BlockType( m_iBlockType ).GetTexture(NORTH, m_iBlockData );
	blockTex /= 16.0f;
	blockTex.y = 1.0f - blockTex.y;
	return {
		blockTex.x,
		blockTex.y,
		blockTex.x + blockTex.z,
		blockTex.y + blockTex.w
	};
}
#endif