#include "blockdef.hpp"
#include "utility/direction.hpp"
#include "utility/vector.hpp"

#ifdef CLIENTEXE
	#include "rendering/blocktexture.hpp"
#endif

#pragma once

class CBlock
{
  public:
	CBlock();

	BLOCKID m_iBlockType = BLOCKID::AIR;
	uint16_t m_iBlockData = 0;

#ifdef CLIENTEXE
	BlockTexture GetSideTexture( Direction side );
#endif

	// Pos is in local coords
	bool TestPointCollision( CVector pos );
	// Pos is in local coords
	bool TestAABBCollision( CVector pos, CVector size );

	// block update
	// causes a mesh rebuild
	void Update();

	// Pointer to chunk that holds us
	void *m_pChunk;
};