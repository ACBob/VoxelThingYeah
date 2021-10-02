#include "particleemitter.hpp"

#include <algorithm>

CParticleEmitter::CParticleEmitter() {}

CParticleEmitter::~CParticleEmitter() {}

void CParticleEmitter::Render(CVector camRot)
{
	for ( CParticle &p : m_particles )
		p.Render(camRot);
}

void CParticleEmitter::PhysicsTick(CWorld *pWorld, int64_t iTick, float fDelta)
{
	m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](auto &&p) { return p.m_fLifeTime <= 0; }), m_particles.end());

	for ( CParticle &p : m_particles )
		p.PhysicsTick(pWorld, fDelta);
	
	if (iTick != m_iLastTick && iTick % m_iParticleEveryNthTick == 0)
	{
		if (m_particles.size() < m_iMaxParticles)
		{
			CParticle &p = m_particles.emplace_back();
			p.m_bCollision = m_bParticlesCollide;
			p.m_fLifeTime = m_fParticleLifetimes;
			p.m_vPosition = m_vPosition;
			p.m_vVelocity = m_vExplosion;
			p.m_vLinear = m_vLinear;
		}

		m_iLastTick = iTick;
	}
}