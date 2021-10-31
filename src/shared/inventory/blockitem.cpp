#include "blockitem.hpp"

CBlockItem::CBlockItem(int count, BLOCKID blockType, BLOCKVAL val) { m_iCount = count; m_blockType = blockType; m_val = val; };
CBlockItem::~CBlockItem(){};

int CBlockItem::GetCount()
{
	if ( m_blockType != BLCK_AIR && m_blockType != BLCK_AIR )
		return BaseClass::GetCount();
	return 0;
}