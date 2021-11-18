// Base block class
// All blocks should extend this!!

#pragma once

#include "types.hpp"
#include "world/physics.hpp"

#include "blockdef.hpp"

#ifdef CLIENTEXE
	#include "rendering/blocktexture.hpp"
#endif

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

	virtual BLOCKMATERIAL GetMaterial() const { return GetBlockMaterial( m_iBlockType ); };

#ifdef CLIENTEXE
	virtual BlockTexture GetTexture( Direction direction, BLOCKVAL metadata ) const
	{
		return GetDefaultBlockTextureSide( m_iBlockType, direction );
	};
	virtual bool FaceVisible( Direction direction, BLOCKID blockId ) const;

	virtual uint16_t GetTint( CChunk *pChunk, Vector3f pos, BLOCKVAL metadata, Direction dir ) const { return 0xFFFF; };
#endif

  protected:
	~CBlockBase() = default;

	const BLOCKID m_iBlockType;
};

const CBlockBase &BlockType( BLOCKID type );