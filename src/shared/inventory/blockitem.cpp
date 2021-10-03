#include "blockitem.hpp"

CBlockItem::CBlockItem() { m_iBlockType = AIR; };
CBlockItem::~CBlockItem(){};

int CBlockItem::GetCount()
{
	if ( m_iBlockType != AIR )
		return BaseClass::GetCount();
	return 0;
}