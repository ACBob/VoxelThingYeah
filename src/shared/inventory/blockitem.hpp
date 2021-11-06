#include "assorted.hpp"
#include "item.hpp"

#include "../blockdef.hpp"

#pragma once

class CBlockItem : public CItem
{
	DeclBaseClass( CItem );

  public:
	CBlockItem(int count = 1, blocktype_t blockType = AIR, uint16_t blockData = 0);
	~CBlockItem();

	// AIR is functionally the same as 0.
	blocktype_t m_iBlockType;

	uint16_t m_iBlockData;

	int GetCount();
};