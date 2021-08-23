#include "utility/vector.hpp"
#include "world/block.hpp"
#include "world/world.hpp"

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