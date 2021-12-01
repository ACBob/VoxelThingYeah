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