#include "particlemanager.hpp"

#include <algorithm>

CParticleManager::CParticleManager()
{

}

CParticleManager::~CParticleManager()
{
	m_particles.clear();
	m_emitters.clear();
}

CParticle &CParticleManager::CreateParticle(CVector pos, ParticleDef pdef)
{
	CParticle &p = m_particles.emplace_back(pdef);
	p.m_vPosition = pos;

	return p;
}

CParticleEmitter &CParticleManager::CreateEmitter(CVector pos, ParticleDef pdef)
{
	CParticleEmitter &p = m_emitters.emplace_back(pdef);
	p.m_vPosition = pos;

	return p;
}

void CParticleManager::PhysicsTick(CWorld *pWorld, int64_t iTick, float fDelta)
{
	m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](auto &&p) { return p.m_fLifeTime <= 0; }), m_particles.end());
	m_emitters.erase(std::remove_if(m_emitters.begin(), m_emitters.end(), [](auto &&p) { return p.m_bDone; }), m_emitters.end());

	for (CParticleEmitter &pe : m_emitters)
		pe.PhysicsTick(pWorld, iTick, fDelta);
	for (CParticle &p : m_particles)
		p.PhysicsTick(pWorld, fDelta);
}

void CParticleManager::Render(CVector camRot)
{
	for (CParticleEmitter &pe : m_emitters)
		pe.Render(camRot);
	for (CParticle &p : m_particles)
		p.Render(camRot);
}