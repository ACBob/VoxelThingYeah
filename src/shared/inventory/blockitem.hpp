#include "assorted.hpp"
#include "item.hpp"

#include "../blockdef.hpp"

#pragma once

class CBlockItem : public CItem
{
	DeclBaseClass( CItem );

  public:
	CBlockItem();
	~CBlockItem();

	// AIR is functionally the same as 0.
	blocktype_t m_iBlockType;

	int GetCount();
};