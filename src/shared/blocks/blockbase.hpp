// Base block class
// All blocks should extend this!!

#pragma once

#include "utility/types.hpp"
#include "world/physics.hpp"

#include "blockdef.hpp"

// Forward decl.
class CChunk;
class CEntityBase;

class CBlockBase
{
  public:
	constexpr CBlockBase( BLOCKID blockType ) : m_iBlockType( blockType ) {}

	// Called when something around this block, or this block itself updates
	// NOTE: vPosition is relative to the chunk!
	virtual void BlockUpdate( CChunk *pChunk, Vector3f vPosition ) const {};

	// Called when the block is placed.
	// NOTE: vPosition is relative to the chunk!
	virtual void OnPlace( CChunk *pChunk, Vector3f vPosition, BLOCKVAL metadata ) const {};

	// Called when the block is broken.
	// NOTE: vPosition is relative to the chunk!
	virtual void OnBreak( CChunk *pChunk, Vector3f vPosition, BLOCKVAL metadata, CEntityBase *pBreaker ) const {};

	// Returns if the block has an action when right-clicked.
	// Future-proofing.
	virtual bool CanBeUsed() const { return false; };

	// Called if CanBeUsed() and right-clicked.
	// NOTE: vPosition is relative to the chunk!
	virtual void OnUse( CChunk *pChunk, Vector3f vPosition, CEntityBase *pUser ) const {};

	// The bounds for the block
	virtual CBoundingBox GetBounds() const;

	virtual bool IsSolid( BLOCKVAL metadata ) const { return true; }
	virtual bool IsSelectable( BLOCKVAL metadata ) const { return true; }

	virtual bool IsTransparent() const { return false; }
	virtual bool IsLiquid() const { return false; }
	virtual bool IsFullCube() const { return true; }

	virtual BLOCKMATERIAL GetMaterial( BLOCKVAL meta ) const { return GetBlockMaterial( m_iBlockType ); };

  protected:
	~CBlockBase() = default;

	const BLOCKID m_iBlockType;
};

const CBlockBase &BlockType( BLOCKID type );