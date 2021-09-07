#include "utility/vector.hpp"

#pragma once

class CBoundingBox
{
  public:
	CBoundingBox( CVector pos, CVector size, CVector origin = CVector( 0.5, 0.5, 0.5 ) );
	bool TestCollide( CBoundingBox other );
	bool TestPointCollide( CVector other );

	CVector m_vPosition;
	CVector m_vBounds;
	CVector m_vOrigin;
};