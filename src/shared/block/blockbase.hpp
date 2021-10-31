// Base block class
// All blocks should extend this!!

#pragma once

#include "types.hpp"
#include "world/physics.hpp"

#include "blockdef.hpp"

// Forward decl.
class CChunk;
class CEntityBase;

class CBlockBase
{
  public:

	// Called when something around this block, or this block itself updates
	// NOTE: vPosition is relative to the chunk!
	virtual void BlockUpdate( CChunk *pChunk, CVector vPosition ) const;

	// Called when the block is placed.
	// NOTE: vPosition is relative to the chunk!
	virtual void OnPlace( CChunk *pChunk, CVector vPosition, BLOCKVAL metadata ) const;

	// Called when the block is broken.
	// NOTE: vPosition is relative to the chunk!
	virtual void OnBreak( CChunk *pChunk, CVector vPosition, BLOCKVAL metadata, CEntityBase *pBreaker ) const;

	// Returns if the block has an action when right-clicked.
	// Future-proofing.
	virtual bool CanBeUsed() const;

	// Called if CanBeUsed() and right-clicked.
	// NOTE: vPosition is relative to the chunk!
	virtual void OnUse( CChunk *pChunk, CVector vPosition, CEntityBase *pUser ) const;

	// The bounds for the block
	virtual CBoundingBox GetBounds() const;


  protected:
	const BLOCKID m_iBlockType;
};