#include "particleemitter.hpp"

CParticleEmitter::CParticleEmitter() {}

CParticleEmitter::~CParticleEmitter() {}

void CParticleEmitter::Render(CVector camForward)
{
	for ( CParticle p : m_particles )
		p.Render(camForward);
}