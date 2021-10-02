#include "particle.hpp"

#include "particlesystem.hpp"

CParticle::CParticle(ParticleDef pdef) :
	m_vSize(1,1),
	m_vVelocity(0),
	m_mdl(particleSystem::particleMdl)
{
	m_particleDef = pdef;

	m_vVelocity = RandomVector(m_particleDef.vMinExplode, m_particleDef.vMaxExplode);
	m_vSize = RandomVector(m_particleDef.vMinSize, m_particleDef.vMaxSize);
	m_vUvOffset = RandomVector(m_particleDef.vUVOffsetMin, m_particleDef.vUVOffsetMax);
	m_fLifeTime = m_particleDef.fMinLifetime + ( m_particleDef.fMaxLifetime - m_particleDef.fMinLifetime ) * ( ( rand() ) / (float)RAND_MAX );
}

CParticle::~CParticle() {}

void CParticle::Render(CVector camRot)
{
	m_mdl->m_vPosition = m_vPosition;
	m_mdl->m_vRotation = camRot;
	m_mdl->m_vSize = m_vSize;
	m_mdl->SetTexture(m_particleDef.pTexture);
	m_mdl->m_vUvOffset = m_vUvOffset;
	m_mdl->Render();
} 

void CParticle::PhysicsTick(CWorld *pWorld, float fDelta)
{
	if (m_fLifeTime <= 0)
		return;

	bool applyFriction = false;
	
	m_fLifeTime -= fDelta;

	m_vPosition.x += m_vVelocity.x * fDelta;
	if ( pWorld->TestPointCollision( m_vPosition ) )
	{
		m_vPosition.x -= m_vVelocity.x * fDelta;
		m_vVelocity.x /= 2;

		applyFriction = true;
	}
	m_vPosition.y += m_vVelocity.y * fDelta;
	if ( pWorld->TestPointCollision( m_vPosition ) )
	{
		m_vPosition.y -= m_vVelocity.y * fDelta;
		m_vVelocity.y /= 2;

		applyFriction = true;
	}
	m_vPosition.z += m_vVelocity.z * fDelta;
	if ( pWorld->TestPointCollision( m_vPosition ) )
	{
		m_vPosition.z -= m_vVelocity.z * fDelta;
		m_vVelocity.z /= 2;

		applyFriction = true;
	}

	m_vVelocity = m_vVelocity + m_particleDef.vLinear;

	CVector f = m_vVelocity * m_particleDef.vDrag;
	if (applyFriction)
		f = f + m_vVelocity * m_particleDef.vFriction;
	m_vVelocity = m_vVelocity - f;



	m_vSize = m_vSize - m_particleDef.vShrinkage * fDelta;

	// Die if we've become a singularity
	if (m_vSize <= CVector(0,0))
		m_fLifeTime = 0.0f;
}