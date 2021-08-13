#include "utility/vector.h"

#pragma once

class AABB 
{
	public:
		AABB(Vector origin, Vector size);
		bool TestCollide(AABB other);
		bool TestPointCollide(Vector other);

		Vector pos;
		Vector bounds;
};