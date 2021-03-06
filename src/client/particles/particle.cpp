#include "particle.hpp"

#include "particlesystem.hpp"

CParticle::CParticle( ParticleDef pdef ) : m_vSize( 1, 1 ), m_vVelocity( 0 ), m_mdl( particleSystem::particleMdl )
{
	m_particleDef = pdef;

	m_vVelocity = RandomVector3<float>( m_particleDef.vMinExplode, m_particleDef.vMaxExplode );
	m_vSize =
		m_particleDef.vMinSize + ( m_particleDef.vMaxSize - m_particleDef.vMinSize ) * ( ( rand() ) / (float)RAND_MAX );
	m_vUvOffset = RandomVector4<float>( m_particleDef.vUVOffsetMin, m_particleDef.vUVOffsetMax );
	m_fLifeTime = m_particleDef.fMinLifetime +
				  ( m_particleDef.fMaxLifetime - m_particleDef.fMinLifetime ) * ( ( rand() ) / (float)RAND_MAX );
}

CParticle::~CParticle() {}

void CParticle::Render( Vector3f camRot )
{
	m_mdl->m_vPosition = m_vPosition;
	// HACK: set the rotation to the inverted camera rotation, but not on the x axis
	m_mdl->m_vRotation = Vector3f( camRot.x, -camRot.y, -camRot.z );
	m_mdl->m_vSize	   = m_vSize;
	m_mdl->SetTexture( m_particleDef.pTexture );
	m_mdl->m_vUvOffset = m_vUvOffset;
	m_mdl->Render();
}

void CParticle::PhysicsTick( CWorld *pWorld, float fDelta )
{
	if ( m_fLifeTime <= 0 )
		return;

	bool applyFriction = false;

	m_fLifeTime -= fDelta;

	if ( m_particleDef.bCollision )
	{
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
	}
	else
	{
		m_vPosition.x += m_vVelocity.x * fDelta;
		m_vPosition.y += m_vVelocity.y * fDelta;
		m_vPosition.z += m_vVelocity.z * fDelta;
	}

	m_vVelocity = m_vVelocity + m_particleDef.vLinear;

	Vector3f f = m_vVelocity * m_particleDef.vDrag;
	if ( applyFriction )
		f = f + m_vVelocity * m_particleDef.vFriction;
	m_vVelocity = m_vVelocity - f;

	m_vSize = m_vSize - m_particleDef.vShrinkage * fDelta;

	// Die if we've become a singularity
	if ( m_vSize.x <= 0 || m_vSize.y <= 0 || m_vSize.z <= 0 )
		m_fLifeTime = 0.0f;

	m_mdl->m_vLighting = pWorld->GetLightingAtWorldPos( m_vPosition );
}