#include "entitybase.hpp"

CEntityBase::CEntityBase() : m_collisionBox( { CVector( 0 ), CVector( 0.5, 0.5, 0.5 ), CVector( 0.5, 0.5, 0.5 ) } ) {}

CEntityBase::~CEntityBase() {}

void CEntityBase::Spawn() {}

void CEntityBase::Kill() { m_bIsKilled = true; }

void CEntityBase::UpdateChildren()
{
	for (CEntityBase *ent : m_children)
	{
		ent->m_pParent = this;
		ent->m_vPosition = m_vPosition + ent->m_vParentPosition.Rotate(m_vRotation);
		ent->m_vVelocity = CVector();
	}
}