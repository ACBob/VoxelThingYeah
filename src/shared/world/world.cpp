#include "world.hpp"

#include "entities/entitybase.hpp"

#include <algorithm>

#ifdef CLIENTEXE
CWorld::CWorld(CShader *blockShader, CShader *entityShader, CShader *waterShader, CTexture *worldTex) {
	m_pWorldTex = worldTex;
	m_pBlockShader = blockShader;
	m_pEntityShader = entityShader;
	m_pLiquidShader = waterShader;
#else
CWorld::CWorld() {
#endif

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

#ifdef CLIENTEXE
	m_chunks[pos]->m_blockModel.SetShader(m_pBlockShader);
	m_chunks[pos]->m_blockModel.SetTexture(m_pWorldTex);
	m_chunks[pos]->m_liquidModel.SetShader(m_pLiquidShader);
	m_chunks[pos]->m_liquidModel.SetTexture(m_pWorldTex);
#endif

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

	pos = pos.Floor();
	c->SetBlockAtLocal( (pos - (pos.Floor() / 16) * 16).Floor(), id, val );
}

std::tuple<BLOCKID, BLOCKVAL> CWorld::TestPointCollision( CVector p )
{
	BLOCKID b = std::get<0>(GetBlockAtWorldPos( p ));
	if ( b == BLCK_NONE )
		return { BLCK_NONE, 0 };
	if ( b == BLCK_AIR )
		return {BLCK_NONE, 0};

	p = p - p.Floor();
	if (CBoundingBox( p.Floor(), CVector( 1, 1, 1 ), CVector( 0 ) ).TestPointCollide( p ))
		return GetBlockAtWorldPos(p);
	else
		return {BLCK_NONE, 0};
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
#ifdef CLIENTEXE
	m_entities.back()->m_pMdl->SetShader(m_pEntityShader);
#endif
	return m_entities.back().get();
}

CEntityBase *CWorld::GetEntityByName( std::string name )
{
	for (std::unique_ptr<CEntityBase> &ent : m_entities)
	{
		CEntityBase *e = ent.get();
		if ( e->m_name == name )
			return e;
	}


	return nullptr;
}

void CWorld::LoadFromData(ChunkData data)
{
	CChunk *c = ChunkAtPosCreate({ (float)data.x, (float)data.y, (float)data.z });

	for (int i = 0; i < CHUNKLENGTH; i++)
	{
		c->SetBlockAtIDX(i, (BLOCKID)data.m_iBlocks[i], data.m_iValue[i]);
	}
}

void CWorld::WorldTick(int nTick, float fDelta)
{
	m_entities.erase(
		std::remove_if( m_entities.begin(), m_entities.end(),
						[]( auto &&c ) { return c->m_bIsKilled;} ), m_entities.end() );

	for (std::unique_ptr<CEntityBase> &ent : m_entities)
		ent->Tick(nTick);
}

#ifdef CLIENTEXE

void CWorld::Render()
{
	// Render regular blocks
	std::map<CVector, std::unique_ptr<CChunk>>::iterator i = m_chunks.begin();
	while ( i != m_chunks.end() )
	{
		i->second->Render();
		i ++;
	}
	// Render entities
	for (std::unique_ptr<CEntityBase> &ent : m_entities)
		ent->Render();
	// Render stuff like water
	i = m_chunks.begin();
	while ( i != m_chunks.end() )
	{
		i->second->RenderLiquid();
		i ++;
	}
}

#endif