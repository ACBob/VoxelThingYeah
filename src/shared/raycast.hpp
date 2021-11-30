#include "vector.hpp"
#include "world/block.hpp"
#include "world/world.hpp"

#pragma once

class CPointedThing
{
  public:
	CPointedThing();

	block_t *m_pBlock = nullptr; // The block at the end
	Vector3f m_vPosition;		 // The position of the hit item
	Vector3f m_vNormal;			 // Facing towards where the ray came from
	float m_fDistance;			 // The distance travelled
};

class CVoxRaycast
{
  public:
	CVoxRaycast();
	// pChunkMan - Pointer to world to access blocks
	// bUseCollision - test for collision if true, if false we just go for the first non-air block
	CPointedThing Cast( CWorld *pChunkMan, bool bUseCollision = true );

	Vector3f m_vPosition;
	Vector3f m_vDirection;
	float m_fLength;
};