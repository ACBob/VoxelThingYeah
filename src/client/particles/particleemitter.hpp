#include "particle.hpp"

#include <vector>

class CParticleEmitter
{
  public:
	CParticleEmitter();
	~CParticleEmitter();

	void Render();

	std::vector<CParticle> m_particles;
};