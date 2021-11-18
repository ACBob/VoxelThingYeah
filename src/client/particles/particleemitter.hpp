#include "particle.hpp"

#include <vector>

#pragma once

class CParticleEmitter
{
  public:
	CParticleEmitter( ParticleDef pdef );
	CParticleEmitter();
	~CParticleEmitter();

	void Render( Vector3f camRot );

	void PhysicsTick( CWorld *pWorld, int64_t iTick, float fDelta );

	Vector3f m_vPosition;

	Vector3f m_vExplosion = { 1, 1, 1 };
	Vector3f m_vLinear	  = { 0, -.98, 0 };

	int64_t m_iLastTick = 0;

	bool m_bDone = false;

	// Properties
	ParticleDef m_particleDef;

	int GetCount();

	float m_fTime		  = 0.0f;
	float m_fNextEmission = 0.0f;

  private:
	std::vector<CParticle> m_particles;
};