#include "utility/vector.hpp"
#include "world/block.hpp"
#include "world/world.hpp"

#pragma once

class CPointedThing
{
  public:
	CPointedThing();

	CBlock *block = nullptr;
	CVector position;
	CVector normal;
};

class CVoxRaycast
{
  public:
	CVoxRaycast();
	CPointedThing Cast( CWorld *chunkMan );

	CVector pos;
	CVector dir;
	float length;
};