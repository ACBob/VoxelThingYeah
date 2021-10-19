#include "entitybase.hpp"
#include "entitycamera.hpp"
#include "shared/inputmanager.hpp"
#include "utility/raycast.hpp"
#include "utility/types.hpp"

#include "inventory/inventory.hpp"

#ifdef CLIENTEXE
	#include "particles/particlemanager.hpp"
#endif

#pragma once

class CEntityPlayer : public CEntityBase
{
	DeclBaseClass( CEntityBase );

  public:
	CEntityPlayer();
	~CEntityPlayer();

	void Tick( int64_t iTick );
	void Spawn()
	{
		BaseClass::Spawn();
#ifdef CLIENTEXE
		m_pMdl = modelSystem::LoadModel( "player.obj" );
		m_pMdl->SetTexture( materialSystem::LoadTexture( "player.png" ) );
#endif
	};
	void Kill() { BaseClass::Kill(); }

#ifdef CLIENTEXE
	void UpdateClient( CWorld *clientSideWorld, CParticleManager *pParticleMan );
#endif

	void PhysicsTick( float delta, CWorld *world );

	bool IsPlayer() { return true; };

#ifdef CLIENTEXE
	CInputManager *m_pInputMan = nullptr;
	CVoxRaycast m_hand;
	CPointedThing m_pointed;

	int m_iSelectedItemIDX = 0;

	CEntityCamera m_camera;

	void *m_pClient = nullptr;
#endif

	CItem *m_pSelectedItem = nullptr;

	bool m_bFly			= true;
	bool m_bInInventory = false;

	CInventory m_inventory;
};