#include "blockitem.hpp"

CBlockItem::CBlockItem(int count, blocktype_t blockType, int valA, int valB) { m_iCount = count; m_iBlockType = blockType; m_iValA = valA; m_iValB = valB; };
CBlockItem::~CBlockItem(){};

int CBlockItem::GetCount()
{
	if ( m_iBlockType != AIR )
		return BaseClass::GetCount();
	return 0;
}