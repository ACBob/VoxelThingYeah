#pragma once

#include "utility/assorted.hpp"
#include "utility/vector.hpp"

#include "world/world.hpp"

#ifdef CLIENTEXE
	#include "rendering/modelmanager.hpp"
	#include "rendering/shadermanager.hpp"
#endif

#include <string>
#include <vector>

class CEntityBase
{
  public:
	CEntityBase(); // stub
	~CEntityBase();

	// Called right after the entity is added to the world
	// Position and rotation are handled already
	virtual void Spawn() = 0;

	// Called when told to remove from the world
	virtual void Kill() = 0;

	// Called every tick (usually 1/20th of a second)
	virtual void Tick( int64_t iTick );

	// Called every tick (usually 1/20th of a second)
	// Gets handed the delta time to help work out physics-related stuff
	// Also gets given the world for collision reasons
	virtual void PhysicsTick( float delta, CWorld *world );

	// Updates the entity's children
	virtual void UpdateChildren();

#ifdef CLIENTEXE
	// Update any client-side things we may have, called every frame rather than per tick
	// Uses the client-side version of the world for testing things (i.e player hand)
	virtual void UpdateClient( CWorld *clientSideWorld ){};

	// Renders our Model.
	virtual void Render()
	{
		m_pMdl->m_vPosition = m_vPosition;
		m_pMdl->m_vRotation = m_vRotation;
		m_pMdl->Render();
	}

	virtual void SetShader( CShader *shader ) { m_pMdl->SetShader( shader ); }

	CModel *m_pMdl;
#endif

	std::string m_name;

	// Gets a forward vector based on the rotation
	CVector GetForward() { return CVector( 0, 0, -1 ).Rotate( m_vRotation ).Normal(); }

	// Updates the collision box to the new position
	void UpdateCollision() { m_collisionBox.m_vPosition = m_vPosition; }

	// Position in world
	CVector m_vPosition;
	// Position relative to parent
	CVector m_vParentPosition;
	// Rotation
	CVector m_vRotation;
	// Velocity
	CVector m_vVelocity;

	bool m_bOnFloor		   = false;
	bool m_bApplyGravity   = true;
	bool m_bFootstepSounds = true;
	bool m_bInWater		   = false;

	// TRUE if it's a player class
	virtual bool IsPlayer() { return false; };

	int64_t m_iFootstepTick = 0;

	CBlock *m_pLastBlockFloor = nullptr;

	CBoundingBox m_collisionBox;

	// if isKilled, is removed from the entity list and not processed otherwise
	bool m_bIsKilled = false;

	// Parent/Child system
	// Parent has authority over its' children
	// If an entity has a parent, it shouldn't change anything by itself really
	// Only things that *should* be changed is m_vParentPosition && m_vRotation
	// Everything else is handled by the parent.
	std::vector<CEntityBase *> m_children;
	CEntityBase *m_pParent = nullptr;
};