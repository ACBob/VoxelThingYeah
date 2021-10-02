#include "rendering/modelmanager.hpp"
#include "vector.hpp"

#include "world/world.hpp"

#pragma once

class CParticle
{

  public:
	CParticle();
	~CParticle();

	void Render(CVector camRot);

	void PhysicsTick(CWorld *pWorld, float fDelta);

	CVector m_vPosition;
	CVector m_vSize; // Z Ignored!!!

	CVector m_vVelocity;

	CVector m_vLinear;
	
	bool m_bCollision;

	float m_fLifeTime;

	CModel *m_mdl;

	bool m_bShouldDie;
};