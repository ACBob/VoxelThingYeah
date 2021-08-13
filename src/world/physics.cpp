#include "physics.h"

AABB::AABB(Vector origin, Vector size)
{
	pos = origin;
	bounds = size;
}

bool AABB::TestCollide(AABB other)
{
	return ((pos + bounds) >= other.pos && pos <= (other.pos + other.bounds));
}

bool AABB::TestPointCollide(Vector pos)
{
	if (pos < this->pos || pos > (this->pos + this->bounds))
		return false; // Out of bounds
	
	// If it's in bounds, it's logically going to collide
	return true;
}