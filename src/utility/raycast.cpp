#include "raycast.hpp"
#include "utility/vector.hpp"
#include "world/world.hpp"

CPointedThing::CPointedThing() : m_pBlock( nullptr ), m_vPosition( 0 ) {}

CVoxRaycast::CVoxRaycast() : m_vPosition( 0 ), m_vDirection( 0 ), m_fLength( 0 ) {}

CPointedThing CVoxRaycast::Cast( CWorld *chunkMan )
{
	CVector vRay = pos;
	CVector vOtherRay; // for figuring out normal

	const float step = 0.01;
	float i			 = 0;

	CBlock *pBlock = nullptr;
	while ( i <= length )
	{
		vOtherRay = vRay;
		vRay	 = m_vPosition + m_vDirection * i;
		i += step;
		b = chunkMan->BlockAtWorldPos( vRay );

		if ( chunkMan->TestPointCollision( vRay ) )
			break;
	}

	CPointedThing pointedThing;
	p.m_vPosition = CVector( ceil( vRay.x ), ceil( vRay.y ), ceil( vRay.z ) );
	p.m_pBlock	   = b;
	p.m_vNormal   = vOtherRay.Floor() - vRay.Floor();
	return pointedThing;
}