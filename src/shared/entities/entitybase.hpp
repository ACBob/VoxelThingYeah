#pragma once

#include "utility/assorted.hpp"
#include "utility/vector.hpp"

#include "world/world.hpp"

#ifdef CLIENTEXE
	#include "rendering/modelmanager.hpp"
	#include "rendering/shadermanager.hpp"
#endif

#include <string>

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
	virtual void Tick(){};

	// Called every tick (usually 1/20th of a second)
	// Gets handed the delta time to help work out physics-related stuff
	// Also gets given the world for collision reasons
	virtual void PhysicsTick( float delta, CWorld *world ) = 0;

#ifdef CLIENTEXE
	// Update any client-side things we may have, called every frame rather than per tick
	// Uses the client-side version of the world for testing things (i.e player hand)
	virtual void UpdateClient( CWorld *clientSideWorld ) = 0;

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

	CVector m_vPosition;
	CVector m_vRotation;
	CVector m_vVelocity;

	CBoundingBox m_collisionBox;

	template <typename S> void serialize( S &s )
	{
		s.container4b( m_vPosition, 3 );
		s.container4b( m_vRotation, 3 );
		s.container4b( m_vVelocity, 3 );
		s.class4b( m_collisionBox );
	};

	// if isKilled, is removed from the entity list and not processed otherwise
	bool m_bIsKilled = false;
};