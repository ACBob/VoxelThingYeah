#include "particle.hpp"

#include "particlesystem.hpp"

CParticle::CParticle()
{
	m_mdl = particleSystem::particleMdl;
}

CParticle::~CParticle() {}

void CParticle::Render(CVector camForward)
{
	m_mdl->m_vPosition = m_vPosition;
	m_mdl->m_vRotation = camForward;
	m_mdl->Render();
}