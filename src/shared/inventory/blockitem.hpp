#include "assorted.hpp"
#include "item.hpp"

#include "../blockdef.hpp"

#pragma once

class CBlockItem : public CItem
{
	DeclBaseClass( CItem );

  public:
	CBlockItem(int count = 1, BLOCKID blockType = BLCK_AIR, BLOCKVAL val = 0);
	~CBlockItem();

	// AIR is functionally the same as 0.
	BLOCKID m_blockType;

	BLOCKVAL m_val;

	int GetCount();
};