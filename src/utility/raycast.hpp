#include "utility/vector.hpp"
#include "world/block.hpp"
#include "world/world.hpp"

#pragma once

class CPointedThing
{
  public:
	CPointedThing();

	CBlock *m_pBlock = nullptr;
	CVector m_vPosition;
	CVector m_vNormal;
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