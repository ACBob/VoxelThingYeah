#include "utility/vector.hpp"

#pragma once

class AABB
{
  public:
	AABB( Vector pos, Vector size, Vector origin = Vector( 0.5, 0.5, 0.5 ) );
	bool TestCollide( AABB other );
	bool TestPointCollide( Vector other );

	Vector pos;
	Vector bounds;
	Vector origin;
};