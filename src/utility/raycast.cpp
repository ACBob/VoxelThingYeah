#include "raycast.hpp"
#include "utility/vector.hpp"
#include "world/world.hpp"

CPointedThing::CPointedThing() : m_vPosition( 0 ) {}

CVoxRaycast::CVoxRaycast() : m_vPosition( 0 ), m_vDirection( 0 ), m_fLength( 0 ) {}

CPointedThing CVoxRaycast::Cast( CWorld *pChunkMan, bool bUseCollision )
{
	CVector vRay = m_vPosition;
	CVector vOtherRay; // for figuring out normal

	const float step = 0.01;
	float i			 = 0;

	std::tuple<BLOCKID, BLOCKVAL> block = { BLCK_NONE, 0 };
	while ( i <= m_fLength )
	{
		vOtherRay = vRay;
		vRay	  = m_vPosition + m_vDirection * i;
		i += step;
		block = pChunkMan->GetBlockAtWorldPos( vRay );

		// TODO:
		// if ( bUseCollision )
		// {
		// 	if ( pChunkMan->TestPointCollision( vRay ) )
		// 		break;
		// }
		// else
		// {
			if ( std::get<0>(block) != BLCK_NONE && std::get<0>(block) != BLCK_AIR )
				break;
		// }
	}

	CPointedThing pointedThing;
	pointedThing.m_vPosition = vRay.Floor();
	pointedThing.m_block	 = std::get<0>(block);
	pointedThing.m_val		 = std::get<1>(block);
	pointedThing.m_vNormal	 = vOtherRay.Floor() - vRay.Floor();
	pointedThing.m_fDistance = i;
	return pointedThing;
}