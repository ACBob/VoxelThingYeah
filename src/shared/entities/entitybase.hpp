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
		mdl->position = position;
		mdl->rotation = rotation;
		mdl->Render();
	}

	virtual void SetShader( CShader *shader ) { mdl->SetShader( shader ); }

	CModel *mdl;
#endif

	std::string name;

	// Gets a forward vector based on the rotation
	CVector GetForward() { return CVector( 0, 0, -1 ).Rotate( rotation ).Normal(); }

	// Updates the collision box to the new position
	void UpdateCollision() { collisionBox.pos = position; }

	CVector position;
	CVector rotation;
	CVector velocity;

	CBoundingBox collisionBox;

	template <typename S> void serialize( S &s )
	{
		s.container4b( position, 3 );
		s.container4b( rotation, 3 );
		s.container4b( velocity, 3 );
		s.class4b( collisionBox );
	};

	// if isKilled, is removed from the entity list and not processed otherwise
	bool isKilled = false;
};