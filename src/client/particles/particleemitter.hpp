#include "particle.hpp"

#include <vector>

class ParticleEmitter
{
	public:
		ParticleEmitter();
		~ParticleEmitter();

		std::vector<Particle> particles;
};