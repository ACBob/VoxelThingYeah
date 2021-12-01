#include "entityplayer.hpp"

#include "network/network.hpp"

#include "inventory/blockitem.hpp"

#include "blocks/blockbase.hpp"

CEntityPlayer::CEntityPlayer() : m_inventory( 36 )
{
	m_vPosition = Vector3f( 0, 0, 0 );

	m_collisionBox.m_vBounds = Vector3f( 0.5, 1.9, 0.5 );
	m_collisionBox.m_vOrigin = Vector3f( 0.5, 0, 0.5 );

	( (CBlockItem *)m_inventory.Slot( 0 ) )->m_iBlockType = STONE;
	( (CBlockItem *)m_inventory.Slot( 0 ) )->SetCount( 64 );

	( (CBlockItem *)m_inventory.Slot( 1 ) )->m_iBlockType = BRICKS;
	( (CBlockItem *)m_inventory.Slot( 1 ) )->SetCount( 64 );
}
CEntityPlayer::~CEntityPlayer() {}

void CEntityPlayer::Tick( int64_t iTick )
{
	BaseClass::Tick( iTick );
}

void CEntityPlayer::PhysicsTick( float delta, CWorld *world ) { BaseClass::PhysicsTick( delta, world ); }