#pragma once

#include "particle.hpp"
#include "particleemitter.hpp"

// Holds all the particles

class CParticleManager
{
  public:
	CParticleManager();
	~CParticleManager();

	void PhysicsTick( CWorld *pWorld, int64_t iTick, float fDelta );

	void Render( Vector3f camRot );

	CParticle *CreateParticle( Vector3f pos, ParticleDef pdef );
	CParticleEmitter *CreateEmitter( Vector3f pos, ParticleDef pdef );

	std::vector<CParticleEmitter> m_emitters;
	std::vector<CParticle> m_particles;
};