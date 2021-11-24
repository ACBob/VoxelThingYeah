#include "blockbase.hpp"

#pragma once

class CBlockTransGeneric : public CBlockBase
{
	using Super = CBlockBase;

  public:
	using Super::Super;

	virtual bool IsTransparent() const { return true; };

#ifdef CLIENTEXE
	virtual bool FaceVisible( Direction direction, BLOCKID blockId ) const
	{
		return blockId != m_iBlockType && ( BlockType( blockId ).IsTransparent() || BlockType( blockId ).IsLiquid() ||
											!BlockType( blockId ).IsFullCube() );
	}
#endif
};