#include "assorted.hpp"
#include "item.hpp"

#include "../blockdef.hpp"

#pragma once

class CBlockItem : public CItem
{
	DeclBaseClass( CItem );

  public:
	CBlockItem(int count = 1, BLOCKID blockType = BLCK_AIR, int valA = 0, int valB = 0);
	~CBlockItem();

	// AIR is functionally the same as 0.
	BLOCKID m_iBlockType;

	uint8_t m_iValA;
	uint8_t m_iValB;

	int GetCount();
};