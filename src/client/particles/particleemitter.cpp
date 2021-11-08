#include "particleemitter.hpp"

#include <algorithm>

CParticleEmitter::CParticleEmitter( ParticleDef pdef ) { m_particleDef = pdef; }
CParticleEmitter::CParticleEmitter( ) { }

CParticleEmitter::~CParticleEmitter() { m_particles.clear(); }

void CParticleEmitter::Render( Vector3f camRot )
{
	for ( CParticle &p : m_particles )
		p.Render( camRot );
}

int CParticleEmitter::GetCount() {
	return m_particles.size();
}

void CParticleEmitter::PhysicsTick( CWorld *pWorld, int64_t iTick, float fDelta )
{
	m_particles.erase(
		std::remove_if( m_particles.begin(), m_particles.end(), []( auto &&p ) { return p.m_fLifeTime <= 0; } ),
		m_particles.end() );

	for ( CParticle &p : m_particles )
		p.PhysicsTick( pWorld, fDelta );

	if ( !m_bDone )
	{
		if ( m_particles.size() < m_particleDef.iMaxParticles )
		{
			for (int i = 0; i < m_particleDef.iEmissionSize; i++)
			{
				CParticle &p  = m_particles.emplace_back( m_particleDef );
				p.m_vPosition = m_vPosition;
			}
		}

		m_iLastTick = iTick;

		m_fNextEmission = m_fTime + m_particleDef.fEmissionRate;
	}

	if ( m_particleDef.bOneShot && m_particles.size() == m_particleDef.iMaxParticles )
		m_bDone = true;
	

	m_fTime += fDelta;
}