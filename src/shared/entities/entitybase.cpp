#include "entitybase.hpp"

#ifdef CLIENTEXE
	#include "sound/soundmanager.hpp"
#endif

CEntityBase::CEntityBase() : m_collisionBox( { CVector( 0 ), CVector( 0.5, 0.5, 0.5 ), CVector( 0.5, 0.5, 0.5 ) } ) {}

CEntityBase::~CEntityBase() {}

void CEntityBase::Spawn() {}

void CEntityBase::Kill() { m_bIsKilled = true; }

void CEntityBase::UpdateChildren()
{
	for ( CEntityBase *ent : m_children )
	{
		ent->m_pParent	 = this;
		ent->m_vPosition = m_vPosition + ent->m_vParentPosition.Rotate( m_vRotation );
		ent->m_vVelocity = CVector();
	}
}

void CEntityBase::PhysicsTick( float fDelta, CWorld *pWorld )
{
	m_bOnFloor = false;

	m_vPosition.x += m_vVelocity.x * fDelta;
	UpdateCollision();
	if ( pWorld->TestAABBCollision( this->m_collisionBox ) )
	{
		m_vPosition.x -= m_vVelocity.x * fDelta;
		m_vVelocity.x /= 2;
	}
	m_vPosition.y += m_vVelocity.y * fDelta;
	UpdateCollision();
	if ( pWorld->TestAABBCollision( this->m_collisionBox ) )
	{
		m_pLastBlockFloor = pWorld->BlockAtWorldPos( m_vPosition );

		m_vPosition.y -= m_vVelocity.y * fDelta;
		m_vVelocity.y /= 2;

		m_bOnFloor = true;
	}
	m_vPosition.z += m_vVelocity.z * fDelta;
	UpdateCollision();
	if ( pWorld->TestAABBCollision( this->m_collisionBox ) )
	{
		m_vPosition.z -= m_vVelocity.z * fDelta;
		m_vVelocity.z /= 2;
	}

	if ( m_bApplyGravity )
		m_vVelocity.y -= ( m_bInWater ? 8.0f : 32.0f ) * fDelta;

	// Terminal Velocity
	m_vVelocity.y = fmaxf( m_vVelocity.y, m_bInWater ? -1.0f : -32.0f );

	CVector vFriction;
	if ( m_bInWater ) // It is hard to move in water
		vFriction = m_vVelocity * 0.5f * fDelta * -1.0f;
	else if ( !m_bOnFloor )
		vFriction = m_vVelocity * 0.11f * fDelta * -1.0f;
	else
		vFriction = m_vVelocity * 0.3f * fDelta * -1.0f;

	m_vVelocity = m_vVelocity + vFriction;

	UpdateChildren();

	// Test if we're in water
	m_bInWater = false;

	CBlock *blockInside = pWorld->BlockAtWorldPos( m_vPosition );
	if ( blockInside != nullptr && ( blockInside->m_iBlockType == WATER || blockInside->m_iBlockType == WATERSRC ) )
		m_bInWater = true;
}

void CEntityBase::Tick( int64_t iTick )
{
#ifdef CLIENTEXE
	CVector t = m_vVelocity;
	t.y		  = 0;
	if ( t.Magnitude() > 1 )
	{
		if ( m_bFootstepSounds && m_bOnFloor && m_pLastBlockFloor != nullptr )
		{
			if ( iTick >= m_iFootstepTick )
			{
				soundSystem::PlayStepSound( m_pLastBlockFloor->m_iBlockType, m_vPosition );
				m_iFootstepTick = iTick + 9;
			}
		}
	}
#endif
}