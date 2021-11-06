#include "blockitem.hpp"

CBlockItem::CBlockItem(int count, BLOCKID blockType, uint16_t blockData ) { m_iCount = count; m_iBlockType = blockType; m_iBlockData = blockData; };
CBlockItem::~CBlockItem(){};

int CBlockItem::GetCount()
{
	if ( m_iBlockType != AIR )
		return BaseClass::GetCount();
	return 0;
}