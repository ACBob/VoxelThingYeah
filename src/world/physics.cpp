#include "physics.h"

AABB::AABB(Vector origin, Vector size)
{
	pos = origin;
	bounds = size;
}

bool AABB::TestCollide(AABB other)
{
	return (other.pos.x + other.bounds.x >= pos.x && other.pos.x <= pos.x + bounds.x) &&
	       (other.pos.y + other.bounds.y >= pos.y && other.pos.y <= pos.y + bounds.y) &&
	       (other.pos.z + other.bounds.z >= pos.z && other.pos.z <= pos.z + bounds.z);
}

bool AABB::TestPointCollide(Vector pos)
{
	if (pos < this->pos || pos > (this->pos + this->bounds))
		return false; // Out of bounds
	
	// If it's in bounds, it's logically going to collide
	return true;
}