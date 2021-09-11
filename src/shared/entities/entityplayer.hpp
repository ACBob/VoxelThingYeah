#include "entitybase.hpp"
#include "entitycamera.hpp"
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
		m_pMdl = modelSystem::LoadModel( "models/player.obj" );
		m_pMdl->SetTexture( materialSystem::LoadTexture( "player.png" ) );
#endif
	};
	void Kill() { BaseClass::Kill(); }

#ifdef CLIENTEXE
	void UpdateClient( CWorld *clientSideWorld );
#endif

	void PhysicsTick( float delta, CWorld *world );

#ifdef CLIENTEXE
	CInputManager *m_pInputMan = nullptr;
	CVoxRaycast m_hand;
	CPointedThing m_pointed;

	// Block type we're gonna place
	blocktype_t m_iSelectedBlockType = blocktype_t::DIRT;

	CEntityCamera m_camera;

	void *m_pClient = nullptr;
#endif

	bool m_bOnFloor;
};