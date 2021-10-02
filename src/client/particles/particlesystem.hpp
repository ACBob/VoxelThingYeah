#include "particleemitter.hpp"
#include "particlemanager.hpp"
#include "rendering/modelmanager.hpp"

#include <vector>

#pragma once

namespace particleSystem
{
	void Init();
	void UnInit();

	extern CModel *particleMdl;
} // namespace particleSystem