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

	void Render( Vector3f camRot );

	void PhysicsTick( CWorld *pWorld, float fDelta );

	Vector3f m_vPosition;
	Vector3f m_vSize; // Z Ignored!!!

	Vector3f m_vVelocity;

	Vector4f m_vUvOffset;

	ParticleDef m_particleDef;

	float m_fLifeTime;

	CModel *m_mdl;

	bool m_bShouldDie;
};