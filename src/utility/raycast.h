#include "utility/vector.h"
#include "world/block.h"
#include "world/world.h"

#pragma once

class PointedThing
{
	public:
		PointedThing();

		Block *block;
		Vector position;
		Vector normal;
};

class VoxRaycast
{
	public:
		VoxRaycast();
		PointedThing Cast(World *chunkMan);

		Vector pos;
		Vector dir;
		float length;

};