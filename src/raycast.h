#include "utility/vector.h"
#include "world/block.h"
#include "world/chunkmanager.h"

#pragma once

class PointedThing
{
	public:
		PointedThing();

		Block *block;
		Vector position;
};

class VoxRaycast
{
	public:
		VoxRaycast();
		PointedThing Cast(ChunkManager *chunkMan);

		Vector pos;
		Vector dir;
		int length;

};