#include "particleemitter.hpp"

#include <vector>

namespace particleSystem
{
	void Init();
	void UnInit();

	extern std::vector<CParticle> particles;
	extern std::vector<CParticleEmitter> particleEmitters;
} // namespace particleSystem