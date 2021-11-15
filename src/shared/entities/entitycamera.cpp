#include "entitycamera.hpp"

CEntityCamera::CEntityCamera() {}
CEntityCamera::~CEntityCamera() {}

void CEntityCamera::PhysicsTick( float delta, CWorld *world )
{
	BaseClass::PhysicsTick( delta, world );

#ifdef CLIENTEXE
	// if we have a parent, offset the camera to bob side to side and up and down
	if ( m_pParent )
	{
		// Minecraft-like camera bobbing
		Vector3f vel = m_pParent->m_vVelocity;
		vel.y		 = 0.0f;
		float speed	 = vel.Magnitude();

		if ( speed > 0.0f && m_pParent->m_bOnFloor )
		{
			m_vParentPosition.y =
				lerp( m_vParentPosition.y, m_fEyeHeight + fabsf( sin( m_fAge * 3 ) ) * 0.1f, delta * speed );
			m_vParentPosition.x = lerp( m_vParentPosition.x, sin( -m_fAge * 3 ) * 0.1f, delta * speed );
		}
		else
		{
			m_vParentPosition.x = lerp( m_vParentPosition.x, 0.0f, 0.9f * delta );
			m_vParentPosition.y = lerp( m_vParentPosition.y, m_fEyeHeight, 0.9f * delta );
		}
	}
#endif
}