#include "particle.hpp"

#include <vector>

#pragma once

class CParticleEmitter
{
  public:
	CParticleEmitter();
	~CParticleEmitter();

	void Render(CVector camForward);

	std::vector<CParticle> m_particles;
};