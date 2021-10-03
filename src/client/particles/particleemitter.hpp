#include "particle.hpp"

#include <vector>

#pragma once

// A particle emitter may not make more than this many particles (seriously dude why)
#define MAXEMITTERPARTICLEMASTER 8192

class CParticleEmitter
{
  public:
	CParticleEmitter( ParticleDef pdef );
	~CParticleEmitter();

	void Render( CVector camRot );

	void PhysicsTick( CWorld *pWorld, int64_t iTick, float fDelta );

	CVector m_vPosition;

	CVector m_vExplosion = { 1, 1, 1 };
	CVector m_vLinear	 = { 0, -.98, 0 };

	int64_t m_iLastTick = 0;

	bool m_bDone = false;

	// Properties
	ParticleDef m_particleDef;

  private:
	std::vector<CParticle> m_particles;
};