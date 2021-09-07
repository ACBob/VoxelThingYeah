#include "physics.hpp"

CBoundingBox::CBoundingBox( CVector pos, CVector size, CVector origin )
{
	this->pos	 = pos;
	this->bounds = size;
	this->origin = origin;
}

bool CBoundingBox::TestCollide( CBoundingBox other )
{
	// This is silly.
	return ( ( other.pos.x - ( other.bounds.x * other.origin.x ) ) + other.bounds.x >=
				 ( pos.x - ( bounds.x * origin.x ) ) &&
			 ( other.pos.x - ( other.bounds.x * other.origin.x ) ) <=
				 ( pos.x - ( other.bounds.x * other.origin.x ) + bounds.x ) ) &&
		   ( ( other.pos.y - ( other.bounds.y * other.origin.y ) ) + other.bounds.y >=
				 ( pos.y - ( bounds.y * origin.y ) ) &&
			 ( other.pos.y - ( other.bounds.y * other.origin.y ) ) <=
				 ( pos.y - ( other.bounds.y * other.origin.y ) + bounds.y ) ) &&
		   ( ( other.pos.z - ( other.bounds.z * other.origin.z ) ) + other.bounds.z >=
				 ( pos.z - ( bounds.z * origin.z ) ) &&
			 ( other.pos.z - ( other.bounds.z * other.origin.z ) ) <=
				 ( pos.z - ( other.bounds.z * other.origin.z ) + bounds.z ) );
}

bool CBoundingBox::TestPointCollide( CVector pos )
{
	if ( pos < this->pos || pos > ( this->pos + this->bounds ) )
		return false; // Out of bounds

	// If it's in bounds, it's logically going to collide
	return true;
}