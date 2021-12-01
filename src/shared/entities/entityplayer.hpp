#include "entitybase.hpp"
#include "entitycamera.hpp"
#include "shared/raycast.hpp"
#include "utility/types.hpp"

#include "inventory/inventory.hpp"
#pragma once

class CEntityPlayer : public CEntityBase
{
	DeclBaseClass( CEntityBase );

  public:
	CEntityPlayer();
	~CEntityPlayer();

	void Tick( int64_t iTick );
	void Spawn( CWorld *world )
	{
		BaseClass::Spawn( world );
	};
	void Kill() { BaseClass::Kill(); }

	void PhysicsTick( float delta, CWorld *world );

	bool IsPlayer() { return true; };

	CItem *m_pSelectedItem = nullptr;

	bool m_bFly			= true;
	bool m_bInInventory = false;
	bool m_bCrouching	= false;

	CInventory m_inventory;
};