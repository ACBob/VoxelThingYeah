#include "entitybase.hpp"

#include "sound/soundmanager.hpp"

CEntityBase::CEntityBase() : m_collisionBox( { Vector3f( 0 ), Vector3f( 0.5, 0.5, 0.5 ), Vector3f( 0.5, 0.5, 0.5 ) } )
{
}

CEntityBase::~CEntityBase() {}

void CEntityBase::Spawn( CWorld *world ) { m_pWorld = world; }

void CEntityBase::Kill() { m_bIsKilled = true; }

void CEntityBase::UpdateChildren()
{
	for ( CEntityBase *ent : m_children )
	{
		ent->m_pParent	 = this;
		ent->m_vPosition = m_vPosition + ent->m_vParentPosition.Rotate( m_vRotation );
		ent->m_vVelocity = Vector3f();
	}
}

void CEntityBase::PhysicsTick( float fDelta, CWorld *pWorld )
{
	m_fAge += fDelta;

	m_bOnFloor = false;
	m_vPosition.y += m_vVelocity.y * fDelta;
	UpdateCollision();
	m_pLastBlockFloor = pWorld->TestAABBCollision( this->m_collisionBox );
	if ( m_pLastBlockFloor )
	{
		if ( m_vVelocity.y >= SMACK_SPEED )
			soundSystem::PlaySoundEvent( "entity.fastsmack", m_vPosition );

		if ( m_vVelocity.y < 0 )
			m_bOnFloor = true;

		m_vPosition.y -= m_vVelocity.y * fDelta;
		m_vVelocity.y /= 2;
	}

	m_vPosition.x += m_vVelocity.x * fDelta;
	UpdateCollision();
	if ( pWorld->TestAABBCollision( this->m_collisionBox ) )
	{
		if ( m_vVelocity.x >= SMACK_SPEED )
			soundSystem::PlaySoundEvent( "entity.fastsmack", m_vPosition );

		// We test against STEP_UP_HEIGHT to see if we can step up
		m_vPosition.y += STEP_UP_HEIGHT;
		UpdateCollision();
		if ( m_bOnFloor && !pWorld->TestAABBCollision( this->m_collisionBox ) )
		{
			m_vVelocity.y = 0;
			m_bOnFloor = true;
		}
		else
		{
			m_vPosition.y -= STEP_UP_HEIGHT;
			m_vPosition.x -= m_vVelocity.x * fDelta;
			m_vVelocity.x /= 2.0f;
		}
	}
	m_vPosition.z += m_vVelocity.z * fDelta;
	UpdateCollision();
	if ( pWorld->TestAABBCollision( this->m_collisionBox ) )
	{
		if ( m_vVelocity.z >= SMACK_SPEED )
			soundSystem::PlaySoundEvent( "entity.fastsmack", m_vPosition );

		// We test against STEP_UP_HEIGHT to see if we can step up
		m_vPosition.y += STEP_UP_HEIGHT;
		UpdateCollision();
		if ( m_bOnFloor && !pWorld->TestAABBCollision( this->m_collisionBox ) )
		{
			m_vVelocity.y = 0;
			m_bOnFloor = true;
		}
		else
		{
			m_vPosition.y -= STEP_UP_HEIGHT;
			m_vPosition.z -= m_vVelocity.z * fDelta;
			m_vVelocity.z /= 2.0f;
		}
	}

	if ( m_bApplyGravity )
		m_vVelocity.y -= ( m_bInWater ? 3.2f : 32.0f ) * fDelta;

	// Terminal Velocity
	m_vVelocity.y = fmaxf( m_vVelocity.y, m_bInWater ? -2.0f : -32.0f );

	Vector3f vFriction;
	if ( m_bInWater ) // It is hard to move in water
		vFriction = m_vVelocity * 0.7f * fDelta * -1.0f;
	else if ( !m_bOnFloor )
		vFriction = m_vVelocity * 0.11f * fDelta * -1.0f;
	else
		vFriction = m_vVelocity * 0.3f * fDelta * -1.0f;

	m_vVelocity = m_vVelocity + vFriction;

	UpdateChildren();

	// Test if we're in water
	m_bInWater = false;

	block_t *blockInside = pWorld->BlockAtWorldPos( m_vPosition );
	if ( blockInside != nullptr && ( blockInside->GetType() == WATER || blockInside->GetType() == WATERSRC ) )
	{
		// if we're in water, and we're going fast enough, make a splash
		if ( !m_bInWater && m_vVelocity.Magnitude() > SPLASH_SPEED )
		{
			soundSystem::PlaySoundEvent( "entity.splash", m_vPosition );
		}

		m_bInWater = true;
	}

#ifdef CLIENTEXE
	m_vLighting = pWorld->GetLightingAtWorldPos( m_vPosition );
#endif
}

void CEntityBase::Tick( int64_t iTick )
{
	Vector3f t = m_vVelocity;
	t.y		   = 0;
	if ( t.Magnitude() > 1 )
	{
		if ( m_bFootstepSounds && m_bOnFloor && m_pLastBlockFloor != nullptr )
		{
			if ( iTick >= m_iFootstepTick )
			{
				soundSystem::PlayStepSound( m_pLastBlockFloor->GetType(), m_vPosition );

				// The time between footsteps is based on the speed of the player
				m_iFootstepTick = iTick + ( 20 - (int)m_vVelocity.Magnitude() );
			}
		}
		else if ( m_bFootstepSounds && m_bInWater )
		{
			if ( iTick >= m_iFootstepTick )
			{
				soundSystem::PlaySoundEvent( "entity.swim", m_vPosition );

				// Time between swimming sounds is based on the speed of the player
				m_iFootstepTick = iTick + ( 18 - (int)m_vVelocity.Magnitude() );
			}
		}
	}
}