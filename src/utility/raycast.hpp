#include "utility/vector.hpp"
#include "world/block.hpp"
#include "world/world.hpp"

#pragma once

class CPointedThing
{
  public:
	CPointedThing();

	BLOCKID m_iBlock = BLCK_NONE; // The block at the end
	CVector m_vPosition;		// The position of the hit item
	CVector m_vNormal;			// Facing towards where the ray came from
	float m_fDistance;			// The distance travelled
};

class CVoxRaycast
{
  public:
	CVoxRaycast();
	// pChunkMan - Pointer to world to access blocks
	// bUseCollision - test for collision if true, if false we just go for the first non-air block
	CPointedThing Cast( CWorld *pChunkMan, bool bUseCollision = true );

	CVector m_vPosition;
	CVector m_vDirection;
	float m_fLength;
};