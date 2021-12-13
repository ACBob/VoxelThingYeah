#pragma once

#include "utility/assorted.hpp"
#include "utility/vector.hpp"

#include "shared/world/world.hpp"

// Move faster than this? Smack!
#define SMACK_SPEED 30

// Move fast enough into water? Splash!
#define SPLASH_SPEED 10

// The block you're moving into is shorter than this? Step up!
#define STEP_UP_HEIGHT 0.55f

#include <string>
#include <vector>

class CEntityBase
{
  public:
	CEntityBase(); // stub
	~CEntityBase();

	// Called right after the entity is added to the world
	// Position and rotation are handled already
	virtual void Spawn( CWorld *world ) = 0;

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

	std::string m_name;

	// Gets a forward vector based on the rotation
	Vector3f GetForward() { return Vector3f( 0, 0, -1 ).Rotate( m_vRotation ).Normal(); }

	// Updates the collision box to the new position
	void UpdateCollision() { m_collisionBox.m_vPosition = m_vPosition; }

	// Position in world
	Vector3f m_vPosition;
	// Position relative to parent
	Vector3f m_vParentPosition;
	// Rotation
	Vector3f m_vRotation;
	// Velocity
	Vector3f m_vVelocity;
	CWorld *m_pWorld;

	bool m_bOnFloor		   = false;
	bool m_bApplyGravity   = true;
	bool m_bFootstepSounds = true;
	bool m_bInWater		   = false;

	float m_fAge = 0.0f;

	// TRUE if it's a player class
	virtual bool IsPlayer() { return false; };

	int64_t m_iFootstepTick = 0;

	block_t *m_pLastBlockFloor = nullptr;

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