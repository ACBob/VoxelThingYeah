#include "utility/vector.hpp"

#include <inttypes.h>

#pragma once

class CBoundingBox
{
  public:
	CBoundingBox( Vector3f pos, Vector3f size, Vector3f origin = Vector3f( 0.5, 0.5, 0.5 ) );
	bool TestCollide( CBoundingBox other ) const;
	bool TestPointCollide( Vector3f other ) const;

	const CBoundingBox &operator*( const Vector3f &scale );
	const CBoundingBox &operator*( float scale );

	Vector3f m_vPosition;
	Vector3f m_vBounds;
	Vector3f m_vOrigin;

	uint16_t m_nLastTouched;
};