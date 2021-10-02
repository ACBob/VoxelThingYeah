#pragma once

#include "particle.hpp"
#include "particleemitter.hpp"

// Holds all the particles

class CParticleManager
{
	public:
		CParticleManager();
		~CParticleManager();

		void PhysicsTick(CWorld *pWorld, int64_t iTick, float fDelta);

		void Render(CVector camRot);

		CParticle &CreateParticle(CVector pos, CVector explode, CVector linear, bool collide);
		CParticleEmitter &CreateEmitter(CVector pos, CVector explode, CVector linear, float lifetime, bool collide, bool oneshot);

		std::vector<CParticleEmitter> m_emitters;
		std::vector<CParticle> m_particles;
};