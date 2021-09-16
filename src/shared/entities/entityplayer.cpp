#include "entityplayer.hpp"

#include "network/network.hpp"

#ifdef CLIENTEXE
	#include "network/client.hpp"
	#include "sound/soundmanager.hpp"
#endif

CEntityPlayer::CEntityPlayer()
{
	m_vPosition = CVector( 0, 0, 0 );
#ifdef CLIENTEXE
	m_hand.m_fLength	= 4;
	m_hand.m_vDirection = GetForward();
	m_hand.m_vPosition	= m_vPosition;

	m_children.push_back( &m_camera );
	m_camera.m_vParentPosition = CVector( 0, 1.72, 0 );
#endif

	m_collisionBox.m_vBounds = CVector( 0.5, 2, 0.5 );
	m_collisionBox.m_vOrigin = CVector( 0.5, 0, 0.5 );
}
CEntityPlayer::~CEntityPlayer() {}

#ifdef CLIENTEXE
void CEntityPlayer::UpdateClient( CWorld *clientSideWorld )
{
	m_pMdl->m_bVisible = false;

	// Mouse Input
	if ( !m_pInputMan->m_bInGui )
	{
		// TODO: sensitivity
		float pitch = m_camera.m_vRotation.x;
		float yaw	= m_camera.m_vRotation.y;

		pitch -= m_pInputMan->m_vMouseMovement.y * 0.1;
		yaw += m_pInputMan->m_vMouseMovement.x * 0.1;

		while ( yaw > 360 )
			yaw -= 360;
		while ( yaw < 0 )
			yaw += 360;

		pitch = pitch < -89.9 ? -89.9 : ( pitch > 89.9 ? 89.9 : pitch );

		m_vRotation.y = yaw;

		m_camera.m_vRotation.x = pitch;
		m_camera.m_vRotation.y = yaw;

		m_hand.m_vPosition	= m_camera.m_vPosition;
		m_hand.m_vDirection = m_camera.GetForward();
		m_pointed			= m_hand.Cast( clientSideWorld );

		if ( m_pInputMan->m_iMouseState & IN_LEFT_MOUSE && m_pInputMan->m_iOldMouseState == 0 &&
			 m_pointed.m_pBlock != nullptr )
		{
			BlockFeatures bF = GetBlockFeatures( m_pointed.m_pBlock->m_iBlockType );
			if ( bF.breakable )
			{
				soundSystem::PlayBreakSound( m_pointed.m_pBlock->m_iBlockType,
											 m_pointed.m_vPosition - CVector( 0.5, 0.5, 0.5 ) );
				m_pointed.m_pBlock->m_iBlockType = blocktype_t::AIR;
				m_pointed.m_pBlock->Update();

				protocol::SendClientSetBlock( ( (CNetworkClient *)m_pClient )->m_pPeer, m_pointed.m_vPosition - 0.5,
											  blocktype_t::AIR );
			}
		}
		if ( m_pInputMan->m_iMouseState & IN_RIGHT_MOUSE && m_pInputMan->m_iOldMouseState == 0 &&
			 m_pointed.m_pBlock != nullptr )
		{
			CBlock *b = clientSideWorld->BlockAtWorldPos( ( m_pointed.m_vPosition - 0.5 ) + m_pointed.m_vNormal );
			BlockFeatures bF = GetBlockFeatures( m_pointed.m_pBlock->m_iBlockType );
			if ( b != nullptr && bF.selectable )
			{
				blocktype_t oldType = b->m_iBlockType;
				b->m_iBlockType		= m_iSelectedBlockType;
				if ( !clientSideWorld->TestAABBCollision( m_collisionBox ) )
				{
					b->Update();
					soundSystem::PlayPlaceSound( b->m_iBlockType, m_pointed.m_vPosition + m_pointed.m_vNormal -
																	  CVector( 0.5, 0.5, 0.5 ) );
				}
				else
					b->m_iBlockType = oldType;

				protocol::SendClientSetBlock( ( (CNetworkClient *)m_pClient )->m_pPeer,
											  ( m_pointed.m_vPosition - 0.5 ) + m_pointed.m_vNormal, b->m_iBlockType );
			}
		}

		if ( m_pInputMan->m_iMouseState & IN_WHEEL_UP )
		{
			m_iSelectedBlockType = blocktype_t( m_iSelectedBlockType + 1 );
			if ( m_iSelectedBlockType == blocktype_t::BEDROCK )
				m_iSelectedBlockType = blocktype_t( m_iSelectedBlockType + 1 );
			if ( m_iSelectedBlockType > blocktype_t::MOSSCBBLE )
				m_iSelectedBlockType = blocktype_t::STONE;
		}
		else if ( m_pInputMan->m_iMouseState & IN_WHEEL_DOWN )
		{
			m_iSelectedBlockType = blocktype_t( m_iSelectedBlockType - 1 );
			if ( m_iSelectedBlockType == blocktype_t::BEDROCK )
				m_iSelectedBlockType = blocktype_t( m_iSelectedBlockType - 1 );
			if ( m_iSelectedBlockType <= blocktype_t::AIR )
				m_iSelectedBlockType = blocktype_t::MOSSCBBLE;
		}
	}
}
#endif

void CEntityPlayer::Tick()
{
	BaseClass::Tick();

#ifdef CLIENTEXE
	if ( m_pInputMan == nullptr )
		return; // This isn't owned by us, don't do anything

	if ( m_pInputMan->m_bInGui )
		return;

	CVector forward = GetForward();
	CVector right	= forward.Rotate( 2, 90 );
	right.y			= 0;
	right			= right.Normal();
	if ( m_pInputMan->m_bInputState[INKEY_FRONT] )
		m_vVelocity = m_vVelocity + ( forward * 2.67 );
	else if ( m_pInputMan->m_bInputState[INKEY_BACK] )
		m_vVelocity = m_vVelocity + ( forward * -2.67 );
	if ( m_pInputMan->m_bInputState[INKEY_LEFT] )
		m_vVelocity = m_vVelocity + ( right * -2.67 );
	else if ( m_pInputMan->m_bInputState[INKEY_RIGHT] )
		m_vVelocity = m_vVelocity + ( right * 2.67 );

	if ( m_pInputMan->m_bInputState[INKEY_UP] )
		m_vVelocity = m_vVelocity + ( VEC_UP * 2.67 );
	else if ( m_pInputMan->m_bInputState[INKEY_DOWN] )
		m_vVelocity = m_vVelocity + ( VEC_UP * -2.67 );
#endif
}

void CEntityPlayer::PhysicsTick( float delta, CWorld *world )
{
	m_vPosition.x += m_vVelocity.x * delta;
	UpdateCollision();
	if ( world->TestAABBCollision( m_collisionBox ) )
	{
		m_vPosition.x -= m_vVelocity.x * delta;
		m_vVelocity.x = 0;
	}
	m_vPosition.y += m_vVelocity.y * delta;
	UpdateCollision();
	if ( world->TestAABBCollision( m_collisionBox ) )
	{
		// Don't glue ourselves to ceilings!
		if ( m_vVelocity.y < 0 )
			m_bOnFloor = true;
		m_vPosition.y -= m_vVelocity.y * delta;
		m_vVelocity.y = 0;
	}
	m_vPosition.z += m_vVelocity.z * delta;
	UpdateCollision();
	if ( world->TestAABBCollision( m_collisionBox ) )
	{
		m_vPosition.z -= m_vVelocity.z * delta;
		m_vVelocity.z = 0;
	}

	m_vVelocity = m_vVelocity * 0.8;

	UpdateChildren();
}