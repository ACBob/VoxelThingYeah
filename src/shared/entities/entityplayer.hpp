#include "camera.hpp"
#include "entitybase.hpp"
#include "shared/inputmanager.hpp"
#include "utility/raycast.hpp"

#pragma once

class CEntityPlayer : public CEntityBase
{
	DeclBaseClass( CEntityBase );

  public:
	CEntityPlayer();
	~CEntityPlayer();

	void Tick();
	void Spawn()
	{
		BaseClass::Spawn();
#ifdef CLIENTEXE
		mdl = modelSystem::LoadModel( "models/player.obj" );
		mdl->SetTexture( materialSystem::LoadTexture( "player.png" ) );
#endif
	};
	void Kill() { BaseClass::Kill(); }

#ifdef CLIENTEXE
	void UpdateClient( CWorld *clientSideWorld );
#endif

	void PhysicsTick( float delta, CWorld *world );

#ifdef CLIENTEXE
	CInputManager *inputMan = nullptr;
	CVoxRaycast hand;
	CPointedThing pointed;

	// Block type we're gonna place
	blocktype_t selectedBlockType = blocktype_t::DIRT;

	CCamera camera;

	void *client = nullptr;
#endif

	bool onFloor;
};