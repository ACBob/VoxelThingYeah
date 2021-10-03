#include "rendering/modelmanager.hpp"
#include "vector.hpp"

#include "world/world.hpp"

#include "particledef.hpp"

#pragma once

class CParticle
{

  public:
	CParticle( ParticleDef pdef );
	~CParticle();

	void Render( CVector camRot );

	void PhysicsTick( CWorld *pWorld, float fDelta );

	CVector m_vPosition;
	CVector m_vSize; // Z Ignored!!!

	CVector m_vVelocity;

	CVector m_vUvOffset;

	ParticleDef m_particleDef;

	float m_fLifeTime;

	CModel *m_mdl;

	bool m_bShouldDie;
};