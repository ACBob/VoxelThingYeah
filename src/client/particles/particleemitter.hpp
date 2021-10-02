#include "particle.hpp"

#include <vector>

#pragma once

class CParticleEmitter
{
  public:
	CParticleEmitter();
	~CParticleEmitter();

	void Render(CVector camRot);

	void PhysicsTick(CWorld *pWorld, int64_t iTick, float fDelta);

	CVector m_vPosition;

	CVector m_vExplosion = {1, 1, 1};
	CVector m_vLinear = {0, -.98, 0};

	int64_t m_iLastTick = 0;

	// Properties
	bool m_bParticlesCollide = true;
	float m_fParticleLifetimes = 2.0;

	int m_iMaxParticles = 100;
	int m_iParticleEveryNthTick = 1;

  private:
	std::vector<CParticle> m_particles;
};