#include "particleemitter.hpp"
#include "rendering/modelmanager.hpp"

#include <vector>

#pragma once

namespace particleSystem
{
	void Init();
	void UnInit();

	extern std::vector<CParticle> particles;
	extern std::vector<CParticleEmitter> particleEmitters;

	extern CModel *particleMdl;
} // namespace particleSystem