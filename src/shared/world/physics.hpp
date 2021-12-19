#include "utility/vector.hpp"

#include <inttypes.h>

#pragma once

class CBoundingBox
{
  public:
	CBoundingBox( Vector3f pos, Vector3f size, Vector3f origin = Vector3f( 0.5, 0.5, 0.5 ) );
	bool TestCollide( CBoundingBox other );
	bool TestPointCollide( Vector3f other );

	Vector3f m_vPosition;
	Vector3f m_vBounds;
	Vector3f m_vOrigin;

	uint16_t m_nLastTouched;
};