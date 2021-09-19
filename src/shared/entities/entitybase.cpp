#include "entitybase.hpp"

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

	// Without this physics feels too fast
	fDelta *= 0.8;

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
		m_vVelocity.y -= 7 * fDelta;

	if ( !m_bOnFloor )
		m_vVelocity = m_vVelocity * CVector( 0.9, 0.99, 0.9 );
	else
		m_vVelocity = m_vVelocity * 0.89;

	UpdateChildren();
}