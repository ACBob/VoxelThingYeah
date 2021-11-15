#include "blockitem.hpp"
#include "blocks/blockbase.hpp"

CBlockItem::CBlockItem( int count, BLOCKID blockType, uint16_t blockData )
{
	m_iCount	 = count;
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

int CBlockItem::GetID() { return m_iBlockType; }
void CBlockItem::SetID( int id )
{
	m_iBlockType = (BLOCKID)id;
	m_iID		 = id;
}

BLOCKVAL CBlockItem::GetData() { return m_iBlockData; }
void CBlockItem::SetData( BLOCKVAL data ) { m_iBlockData = data; }

#ifdef CLIENTEXE
CTexture *CBlockItem::GetTexture() { return materialSystem::LoadTexture( "terrain.png" ); }
Vector4f CBlockItem::GetUV()
{
	Vector4f blockTex = BlockType( m_iBlockType ).GetTexture( NORTH, m_iBlockData );
	blockTex /= 16.0f;
	return { blockTex.x, blockTex.y, blockTex.x + blockTex.z, blockTex.y + blockTex.w };
}
CColour CBlockItem::GetTint()
{
	return BlockType( m_iBlockType ).GetTint( nullptr, {}, m_iBlockData, NORTH );
}
#endif