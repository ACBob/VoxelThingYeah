#include "particle.hpp"

CParticle::CParticle() {}

CParticle::~CParticle() {}

void CParticle::Render()
{
	m_mdl->m_vPosition = m_vPosition;
	m_mdl->Render();
}