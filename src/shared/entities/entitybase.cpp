#include "entitybase.hpp"

CEntityBase::CEntityBase() : m_collisionBox( { CVector( 0 ), CVector( 1, 1, 1 ), CVector( 0.5, 0.5, 0.5 ) } ) {}

CEntityBase::~CEntityBase() {}

void CEntityBase::Spawn() {}

void CEntityBase::Kill() { m_bIsKilled = true; }