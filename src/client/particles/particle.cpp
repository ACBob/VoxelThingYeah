#include "particle.hpp"

#include "particlesystem.hpp"

CParticle::CParticle() :
	m_vSize(1,1),
	m_vVelocity(0)
{
	m_mdl = particleSystem::particleMdl;
}

CParticle::~CParticle() {}

void CParticle::Render(CVector camRot)
{
	m_mdl->m_vPosition = m_vPosition;
	m_mdl->m_vRotation = camRot;
	// m_mdl->m_vSize = m_vSize;
	m_mdl->Render();
} 

void CParticle::PhysicsTick(CWorld *pWorld, float fDelta)
{
	if (m_fLifeTime <= 0)
		return;
	
	m_fLifeTime -= fDelta;

	m_vPosition.x += m_vVelocity.x * fDelta;
	if ( pWorld->TestPointCollision( m_vPosition ) )
	{
		m_vPosition.x -= m_vVelocity.x * fDelta;
		m_vVelocity.x /= 2;
	}
	m_vPosition.y += m_vVelocity.y * fDelta;
	if ( pWorld->TestPointCollision( m_vPosition ) )
	{
		m_vPosition.y -= m_vVelocity.y * fDelta;
		m_vVelocity.y /= 2;
	}
	m_vPosition.z += m_vVelocity.z * fDelta;
	if ( pWorld->TestPointCollision( m_vPosition ) )
	{
		m_vPosition.z -= m_vVelocity.z * fDelta;
		m_vVelocity.z /= 2;
	}

	m_vVelocity = m_vVelocity + m_vLinear;
}