#include "raycast.hpp"
#include "utility/vector.hpp"
#include "world/world.hpp"

CPointedThing::CPointedThing() : m_pBlock( nullptr ), m_vPosition( 0 ) {}

CVoxRaycast::CVoxRaycast() : m_vPosition( 0 ), m_vDirection( 0 ), m_fLength( 0 ) {}

CPointedThing CVoxRaycast::Cast( CWorld *pChunkMan, bool bUseCollision )
{
	CVector vRay = m_vPosition;
	CVector vOtherRay; // for figuring out normal

	const float step = 0.01;
	float i			 = 0;

	CBlock *pBlock = nullptr;
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
			if ( pBlock != nullptr && pBlock->m_iBlockType != AIR )
				break;
		}
	}

	CPointedThing pointedThing;
	pointedThing.m_vPosition = CVector( ceil( vRay.x ), ceil( vRay.y ), ceil( vRay.z ) );
	pointedThing.m_pBlock	 = pBlock;
	pointedThing.m_vNormal	 = vOtherRay.Floor() - vRay.Floor();
	pointedThing.m_fDistance = i;
	return pointedThing;
}