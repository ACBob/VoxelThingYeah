#include "world.hpp"

#include <algorithm>

CWorld::CWorld()
{

}

CWorld::~CWorld()
{
	m_chunks.clear();
}

CChunk *CWorld::ChunkAtPosNoCreate(CVector pos)
{
	if (m_chunks.find(pos) == m_chunks.end())
		return nullptr;
	
	return m_chunks.at(pos).get();
}

CChunk *CWorld::ChunkAtPosCreate(CVector pos)
{
	CChunk *c = ChunkAtPosNoCreate(pos);
	if (c)
		return c;

	m_chunks[pos] = std::make_unique<CChunk>();
	return m_chunks[pos].get();
}

CChunk *CWorld::ChunkAtWorldPosNoCreate(CVector pos)
{
	pos = { pos.x / CHUNKSIZE_X, pos.y / CHUNKSIZE_Y, pos.z / CHUNKSIZE_Z };
	pos = pos.Floor();

	return ChunkAtPosNoCreate(pos);
}

CChunk *CWorld::ChunkAtWorldPos(CVector pos)
{
	pos = { pos.x / CHUNKSIZE_X, pos.y / CHUNKSIZE_Y, pos.z / CHUNKSIZE_Z };
	pos = pos.Floor();

	return ChunkAtPosCreate(pos);
}

std::tuple<BLOCKID, BLOCKVAL> CWorld::GetBlockAtWorldPos( CVector pos )
{
	CChunk *c = ChunkAtWorldPosNoCreate(pos);
	if (c == nullptr)
		return { BLCK_NONE, 0 };

	return c->GetBlockAtLocal( (pos - (pos.Floor() / 16) * 16).Floor() );
}

void CWorld::SetBlockAtWorldPos( CVector pos, BLOCKID id, BLOCKVAL val )
{
	CChunk *c = ChunkAtWorldPosNoCreate(pos);
	if (c == nullptr)
		return;

	c->SetBlockAtLocal( (pos - (pos.Floor() / 16) * 16).Floor(), id, val );
}

CEntityBase *CWorld::AddEntity(std::unique_ptr<CEntityBase> ent)
{
	m_entities.push_back( std::move(ent) );
	return m_entities.back().get();
}

void CWorld::WorldTick(int nTick, float fDelta)
{
	m_entities.erase(
		std::remove_if( m_entities.begin(), m_entities.end(),
						[]( auto &&c ) { return c->m_bIsKilled;} ), m_entities.end() );

	for (std::unique_ptr<CEntityBase> &ent : m_entities)
		ent->Tick(nTick);
}