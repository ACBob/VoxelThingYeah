#pragma once

#include "blockbase.hpp"

class CBlockLiquid final : public CBlockBase
{
	using Super = CBlockBase;

  public:
	using Super::Super;

	virtual bool IsSolid( BLOCKVAL metadata ) const { return false; };

#ifdef CLIENTEXE
	virtual bool FaceVisible( Direction direction, BLOCKID blockId ) const
	{
		return blockId != m_iBlockType && ( BlockType( blockId ).IsTransparent() || BlockType( blockId ).IsLiquid() ||
											!BlockType( blockId ).IsFullCube() );
	}

	virtual bool IsFullCube() const { return false; };
	virtual bool IsTransparent() const { return true; };
	virtual bool IsLiquid() const { return true; };
#endif
};