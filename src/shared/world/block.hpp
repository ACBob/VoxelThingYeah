// TODO: is this class neccesary? Seems bloaty!

#include "blockdef.hpp"
#include "utility/direction.hpp"
#include "utility/vector.hpp"

#ifdef CLIENTEXE
	#include "rendering/blocktexture.hpp"
#endif

#pragma once

// Forward Decl.
class CChunk;

// Essentially blocks are a (hopefully) tiny struct that contains the data,
// And some helper functions.
struct block_t
{
  public:
  // Block data gives us
  // 2 bytes for the block type
  // 2 bytes for the block meta
  // Block type is stored on the first two bytes
  // Block meta is stored on the last two bytes
	uint32_t blck = 0;

	void Set(uint16_t type, uint16_t meta = 0)
	{
		blck = (type << 16) | meta;
		Update();
	}

	void Get(uint16_t &type, uint16_t &meta)
	{
		type = blck >> 16;
		meta = blck & 0xFFFF;
	}

	uint16_t GetType()
	{
		return blck >> 16;
	}

	uint16_t GetMeta()
	{
		return blck & 0xFFFF;
	}

#ifdef CLIENTEXE
	BlockTexture GetSideTexture( Direction side );
#endif

	// Pos is in local coords
	bool TestPointCollision( Vector3f pos );
	// Pos is in local coords
	bool TestAABBCollision( Vector3f pos, Vector3f size );

	// block update, marks the chunk as dirty
	// WARNING: Transitionary, will be removed soon
	void Update();

	// Pointer to chunk that holds us
	CChunk *m_pChunk;
};