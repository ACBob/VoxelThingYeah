#include "utility/assorted.hpp"
#include "item.hpp"

#include "../shared/blockdef.hpp"

#pragma once

class CBlockItem : public CItem
{
	DeclBaseClass( CItem );

  public:
	CBlockItem( int count = 1, BLOCKID blockType = AIR, uint16_t blockData = 0 );
	~CBlockItem();

	int GetID();
	void SetID( int id );

	BLOCKVAL GetData();
	void SetData( BLOCKVAL data );

	// AIR is functionally the same as 0.
	BLOCKID m_iBlockType;

	uint16_t m_iBlockData;

	int GetCount();
};