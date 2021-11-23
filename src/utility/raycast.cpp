#include "raycast.hpp"
#include "utility/vector.hpp"
#include "world/world.hpp"

CPointedThing::CPointedThing() : m_pBlock( nullptr ), m_vPosition( 0 ) {}

CVoxRaycast::CVoxRaycast() : m_vPosition( 0 ), m_vDirection( 0 ), m_fLength( 0 ) {}

CPointedThing CVoxRaycast::Cast( CWorld *pChunkMan, bool bUseCollision )
{
	Vector3f vRay = m_vPosition;
	Vector3f vOtherRay; // for figuring out normal

	const float step = 0.01;
	float i			 = 0;

	block_t *pBlock = nullptr;
	while ( i <= m_fLength )
	{
		vOtherRay = vRay;
		vRay	  = m_vPosition + m_vDirection * i;
		i += step;
		pBlock = pChunkMan->BlockAtWorldPos( vRay );

		if ( bUseCollision )
		{
			if ( pChunkMan->TestPointCollision( vRay ) )
				break;
		}
		else
		{
			if ( pBlock != nullptr && pBlock->GetType() != AIR )
				break;
		}
	}

	CPointedThing pointedThing;
	pointedThing.m_vPosition = Vector3f( ceil( vRay.x ), ceil( vRay.y ), ceil( vRay.z ) );
	pointedThing.m_pBlock	 = pBlock;
	pointedThing.m_vNormal	 = vOtherRay.Floor() - vRay.Floor();
	pointedThing.m_fDistance = i;
	return pointedThing;
}