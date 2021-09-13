#include "particleemitter.hpp"

CParticleEmitter::CParticleEmitter() {}

CParticleEmitter::~CParticleEmitter() {}

void CParticleEmitter::Render()
{
	for ( CParticle p : m_particles )
		p.Render();
}