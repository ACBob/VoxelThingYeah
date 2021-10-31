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

	m_chunks[pos] = std::make_unique<CChunk>(pos, this);
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

std::tuple<CVector, BLOCKID> CWorld::TestAABBCollision( CBoundingBox col )
{
	CChunk *chunk = ChunkAtWorldPos( col.m_vPosition );
	if ( chunk == nullptr )
		return { {0, 0, 0, 1}, BLCK_NONE };

	// Test A (pos)
	for ( int i = 0; i < ( CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z ); i++ )
	{
		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );

		// Don't collide with air
		std::tuple<BLOCKID, BLOCKVAL> block = chunk->GetBlockAtIDX(i);
		if ( std::get<0>(block) == BLCK_AIR )
			continue;

		if ( col.TestCollide(
				 CBoundingBox( chunk->GetPosInWorld( CVector( x, y, z ) ), CVector( 1, 1, 1 ), CVector( 0 ) ) ) )
			return { chunk->GetPosInWorld( CVector( x, y, z ) ),  std::get<0>(block) };
	}

	return { {0, 0, 0, 1}, BLCK_NONE };
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