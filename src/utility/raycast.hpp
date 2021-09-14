#include "utility/vector.hpp"
#include "world/block.hpp"
#include "world/world.hpp"

#pragma once

class CPointedThing
{
  public:
	CPointedThing();

	CBlock *m_pBlock = nullptr; // The block at the end
	CVector m_vPosition; // The position of the hit item
	CVector m_vNormal; // Facing towards where the ray came from
	float m_fDistance; // The distance travelled
};

class CVoxRaycast
{
  public:
	CVoxRaycast();
	CPointedThing Cast( CWorld *pChunkMan );

	CVector m_vPosition;
	CVector m_vDirection;
	float m_fLength;
};