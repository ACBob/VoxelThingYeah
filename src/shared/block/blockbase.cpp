#include "blockbase.hpp"
#include "blockair.hpp"

#include "world/chunk.hpp"

#include "entities/entitybase.hpp"


void CBlockBase::BlockUpdate( CChunk *pChunk, CVector vPosition ) const {}

void CBlockBase::OnPlace( CChunk *pChunk, CVector vPosition, BLOCKVAL metadata ) const {}

void CBlockBase::OnBreak( CChunk *pChunk, CVector vPosition, BLOCKVAL metadata, CEntityBase *pBreaker ) const {}

bool CBlockBase::CanBeUsed() const {
    return false;
}

void CBlockBase::OnUse( CChunk *pChunk, CVector vPosition, CEntityBase *pUser ) const {}

CBoundingBox CBlockBase::GetBounds() const {
    return CBoundingBox( CVector(0), CVector( 1, 1, 1 ), CVector( 0 ) );
}