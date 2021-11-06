#include "entitycamera.hpp"

CEntityCamera::CEntityCamera() {}
CEntityCamera::~CEntityCamera() {}

void CEntityCamera::PhysicsTick( float delta, CWorld *world )
{
    BaseClass::PhysicsTick( delta, world );

    // if we have a parent, offset the camera to bob side to side and up and down
    if (m_pParent)
    {
        // Minecraft-like camera bobbing
        CVector vel = m_pParent->m_vVelocity;
        vel.y = 0.0f;
        float speed = vel.Magnitude();
        
        if (speed > 0.0f)
        {
            m_vParentPosition.x = (sin(m_fAge * 2 + m_fAge * 0.5) - 0.5) * 0.007 * speed;
            m_vParentPosition.y = 1.72 + (fabs(sin(m_fAge * 2 + m_fAge * 0.25)) - 0.5) * 0.01 * speed;
        }
        else
        {
            m_vParentPosition.x = lerp(m_vParentPosition.x, 0.0f, 0.9f * delta);
            m_vParentPosition.y = lerp(m_vParentPosition.y, 1.72f, 0.9f * delta);
        }
    }
}