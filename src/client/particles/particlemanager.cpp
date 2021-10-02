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

CParticle &CParticleManager::CreateParticle(CVector pos, CVector explode, CVector linear, bool collide)
{
	CParticle &p = m_particles.emplace_back();
	p.m_vPosition = pos;
	p.m_vVelocity = explode;
	p.m_vLinear = linear;
	p.m_bCollision = collide;

	return p;
}

CParticleEmitter &CParticleManager::CreateEmitter(CVector pos, CVector explode, CVector linear, float lifetime, bool collide, bool oneshot)
{
	CParticleEmitter &p = m_emitters.emplace_back();
	p.m_vPosition = pos;
	p.m_vExplosion = explode;
	p.m_vLinear = linear;
	p.m_bParticlesCollide = collide;
	p.m_fParticleLifetimes = lifetime;
	p.m_bOneshot = oneshot;

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