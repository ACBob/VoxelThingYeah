#include "assorted.hpp"
#include "item.hpp"

#include "../blockdef.hpp"

#pragma once

class CBlockItem : public CItem
{
	DeclBaseClass( CItem );

  public:
	CBlockItem(int count = 1, blocktype_t blockType = AIR, int valA = 0, int valB = 0);
	~CBlockItem();

	// AIR is functionally the same as 0.
	blocktype_t m_iBlockType;

	uint8_t m_iValA;
	uint8_t m_iValB;

	int GetCount();
};