#include "particleemitter.hpp"

#include <vector>

namespace particleSystem
{
	void Init();
	void UnInit();

	extern std::vector<Particle> particles;
	extern std::vector<ParticleEmitter> particleEmitters;
}