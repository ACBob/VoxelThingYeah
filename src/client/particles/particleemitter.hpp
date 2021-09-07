#include "particle.hpp"

#include <vector>

class CParticleEmitter
{
  public:
	CParticleEmitter();
	~CParticleEmitter();

	std::vector<CParticle> m_particles;
};