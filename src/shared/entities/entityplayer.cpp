#include "entityplayer.hpp"

#include "network/network.hpp"

#ifdef CLIENTEXE
	#include "network/client.hpp"
	#include "sound/soundmanager.hpp"
#endif

#include "inventory/blockitem.hpp"

#include "blocks/blockbase.hpp"

CEntityPlayer::CEntityPlayer() : m_inventory( 36 )
{
	m_vPosition = Vector3f( 0, 0, 0 );
#ifdef CLIENTEXE
	m_hand.m_fLength	= 4;
	m_hand.m_vDirection = GetForward();
	m_hand.m_vPosition	= m_vPosition;

	m_children.push_back( &m_camera );
	m_camera.m_vParentPosition = Vector3f( 0, 1.72, 0 );
#endif

	m_collisionBox.m_vBounds = Vector3f( 0.5, 1.9, 0.5 );
	m_collisionBox.m_vOrigin = Vector3f( 0.5, 0, 0.5 );
}
CEntityPlayer::~CEntityPlayer() {}

#ifdef CLIENTEXE
void CEntityPlayer::UpdateClient( CWorld *clientSideWorld, CParticleManager *pParticleMan )
{
	m_pMdl->m_bVisible = false;

	// Mouse Input
	if ( !m_pInputMan->m_bInGui )
	{
		// TODO: sensitivity
		float pitch = m_camera.m_vRotation.x;
		float yaw	= m_camera.m_vRotation.y;

		pitch -= m_pInputMan->m_vMouseMovement.y * 0.1;
		yaw -= m_pInputMan->m_vMouseMovement.x * 0.1;

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
			// BlockFeatures bF = GetBlockFeatures( m_pointed.m_pBlock->m_iBlockType );
			// if ( bF.breakable )
			// {
			m_pointed.m_pBlock->m_iBlockType = BLOCKID::AIR;
			m_pointed.m_pBlock->Update();

			protocol::SendClientSetBlock( ( (CNetworkClient *)m_pClient )->m_pPeer, m_pointed.m_vPosition - 0.5,
											BLOCKID::AIR, 0 );
			// }
		}
		if ( m_pInputMan->m_iMouseState & IN_RIGHT_MOUSE && m_pInputMan->m_iOldMouseState == 0 &&
			 m_pointed.m_pBlock != nullptr )
		{
			if (BlockType(m_pointed.m_pBlock->m_iBlockType).CanBeUsed())
			{
				protocol::SendClientUseBlock( ( (CNetworkClient *)m_pClient )->m_pPeer, m_pointed.m_vPosition - 0.5 );
			}
			else
			{
				CBlock *b = clientSideWorld->BlockAtWorldPos( ( m_pointed.m_vPosition - 0.5 ) + m_pointed.m_vNormal );
				if ( m_pSelectedItem != nullptr && m_pSelectedItem->GetCount() > 0 && b != nullptr )
				{
					BLOCKID oldType = b->m_iBlockType; // TODO: We're assuming it's a block item
					uint16_t oldData = b->m_iBlockData;
					CBlockItem *blckItem = reinterpret_cast<CBlockItem *>( m_pSelectedItem );
					b->m_iBlockType		= blckItem->m_iBlockType;
					b->m_iBlockData = blckItem->m_iBlockData;
					if ( !clientSideWorld->TestAABBCollision( m_collisionBox ) )
					{
						b->Update();
						m_pSelectedItem->SetCount( m_pSelectedItem->GetCount() - 1 );

						protocol::SendClientSetBlock( ( (CNetworkClient *)m_pClient )->m_pPeer, ( m_pointed.m_vPosition - 0.5 ) + m_pointed.m_vNormal, b->m_iBlockType, b->m_iBlockData );
					}
					else
					{
						b->m_iBlockType = oldType;
						b->m_iBlockData = oldData;
					}
				}
			}
		}

		if ( m_pInputMan->m_iMouseState & IN_WHEEL_UP )
		{
			m_iSelectedItemIDX--;
			m_iSelectedItemIDX = m_iSelectedItemIDX >= 0 ? m_iSelectedItemIDX : 8;
		}
		else if ( m_pInputMan->m_iMouseState & IN_WHEEL_DOWN )
		{
			m_iSelectedItemIDX++;
			m_iSelectedItemIDX = m_iSelectedItemIDX < 9 ? m_iSelectedItemIDX : 0;
		}

		m_pSelectedItem = m_inventory.Slot( m_iSelectedItemIDX );
	}
}
#endif

void CEntityPlayer::Tick( int64_t iTick )
{
	BaseClass::Tick( iTick );

#ifdef CLIENTEXE
	if ( m_pInputMan == nullptr )
	{
		// Don't apply gravity to players not owned by us (we'll get their pos from the server)
		m_bApplyGravity = false;
		return; // This isn't owned by us, don't do anything
	}

	Vector3f forward = GetForward();
	Vector3f left	= forward.RotateAxis( 1, 90 * DEG2RAD );
	left.y			= 0;
	left			= left.Normal();

	Vector3f vMoveDir( 0 );

	if ( !m_bInInventory && !m_pInputMan->m_bInGui )
	{

		if ( m_pInputMan->m_bInputState[INKEY_FLY] && !m_pInputMan->m_bOldInputState[INKEY_FLY] )
			m_bFly = !m_bFly;

		if ( m_pInputMan->m_bInputState[INKEY_FRONT] )
			vMoveDir = vMoveDir + ( forward );
		else if ( m_pInputMan->m_bInputState[INKEY_BACK] )
			vMoveDir = vMoveDir - ( forward );
		if ( m_pInputMan->m_bInputState[INKEY_LEFT] )
			vMoveDir = vMoveDir + ( left );
		else if ( m_pInputMan->m_bInputState[INKEY_RIGHT] )
			vMoveDir = vMoveDir - ( left );

		if ( m_bFly || m_bInWater )
		{
			if ( m_pInputMan->m_bInputState[INKEY_UP] )
				vMoveDir = vMoveDir + ( VEC_UP );
			else if ( m_pInputMan->m_bInputState[INKEY_DOWN] )
				vMoveDir = vMoveDir - ( VEC_UP );
		}
		else
		{
			if ( m_pInputMan->m_bInputState[INKEY_UP] && m_bOnFloor )
			{
				vMoveDir.y = 19.8f;
			}
		}

		m_bCrouching = m_pInputMan->m_bInputState[INKEY_DOWN] && m_bOnFloor;

		float fSpeed = m_bFly ? 6.5f : m_bInWater ? 2.3f : m_bCrouching ? 1.43f : 4.3f;
		vMoveDir = vMoveDir * fSpeed * 0.98f;

		if ( !m_bFly )
			vMoveDir.y += m_vVelocity.y;
	}

	float acceleration = m_bInWater ? 0.09f : m_bOnFloor ? 0.125f : m_bFly ? 0.1f : 0.076f;

	m_vVelocity = m_vVelocity.Lerp( vMoveDir, acceleration );

	m_camera.m_fEyeHeight = m_bCrouching ? 1.595f : 1.72f;

	m_bApplyGravity = !m_bFly;
#endif
}

void CEntityPlayer::PhysicsTick( float delta, CWorld *world ) { BaseClass::PhysicsTick( delta, world ); }