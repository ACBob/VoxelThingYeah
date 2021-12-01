#include "world.hpp"

#include "entities/entitybase.hpp"

#include "physics.hpp"

#include "shared/logging.hpp"

#include <algorithm>

#include "blocks/blockbase.hpp"

CWorld::CWorld()
{
}
CWorld::~CWorld()
{
	// Destroy chunks
	m_chunks.clear();
}

// Return in good faith that it's a valid position
CChunk *CWorld::ChunkAtChunkPos( Vector3f pos )
{
	// TODO: std::map
	for ( auto &&c : m_chunks )
		if ( c.get()->m_vPosition == pos )
			return c.get();

	return nullptr;
}

// Tries to get a chunk and generates a new one if it can't find one
CChunk *CWorld::GetChunkGenerateAtWorldPos( Vector3f pos )
{
	return GetChunkGenerateAtPos( ( pos / Vector3f( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ).Floor() );
}

CChunk *CWorld::GetChunkGenerateAtPos( Vector3f pos )
{
	// pos is in chunk space
	CChunk *c = ChunkAtChunkPos( pos );
	if ( c != nullptr )
		return c;

	m_chunks.push_back( std::make_unique<CChunk>() );
	c				   = m_chunks.back().get();
	c->m_vPosition	   = pos;
	c->m_portableDef.x = pos.x;
	c->m_portableDef.y = pos.y;
	c->m_portableDef.z = pos.z;
	c->m_pChunkMan	   = this;

	// test if there's any buffer blocks that intersect with this chunk
	for ( auto &&b : m_blocksToPlace )
	{
		if ( b.first.x >= c->m_vPosition.x * CHUNKSIZE_X && b.first.x < ( c->m_vPosition.x + 1 ) * CHUNKSIZE_X &&
			 b.first.y >= c->m_vPosition.y * CHUNKSIZE_Y && b.first.y < ( c->m_vPosition.y + 1 ) * CHUNKSIZE_Y &&
			 b.first.z >= c->m_vPosition.z * CHUNKSIZE_Z && b.first.z < ( c->m_vPosition.z + 1 ) * CHUNKSIZE_Z )
		{
			block_t *blck = c->GetBlockAtLocal( b.first );

			if ( blck != nullptr )
			{
				blck->Set( b.second, 0 );
			}
		}
	}

	// TODO: remove the buffer blocks that intersect with this chunk
	// Using std::remove_if is a bit of a pain, but it's the only way I can think of to do this
	// m_blocksToPlace.erase(
	// 	std::remove_if( m_blocksToPlace.begin(), m_blocksToPlace.end(),
	// 					[c]( const std::pair<Vector3i, uint32_t> &b ) {
	// 						return b.first.x >= c->m_vPosition.x * CHUNKSIZE_X && b.first.x < (c->m_vPosition.x + 1) * CHUNKSIZE_X
	// && 							   b.first.y >= c->m_vPosition.y * CHUNKSIZE_Y && b.first.y < (c->m_vPosition.y + 1) * CHUNKSIZE_Y && 							   b.first.z
	// >= c->m_vPosition.z * CHUNKSIZE_Z && b.first.z < (c->m_vPosition.z + 1) * CHUNKSIZE_Z; 					} ), 	m_blocksToPlace.end()
	// );

	c->m_bReallyDirty = c->m_bDirty = true;

	// rebuild the portable as it's air now
	c->RebuildPortable();

	return c;
}

void CWorld::UnloadChunk( Vector3f pos )
{
	m_chunks.erase(
		std::remove_if( m_chunks.begin(), m_chunks.end(), [pos]( auto &&c ) { return c.get()->m_vPosition == pos; } ),
		m_chunks.end() );
}

// Return in good faith that it's a valid position
CChunk *CWorld::ChunkAtWorldPos( Vector3f pos )
{
	pos = pos / Vector3f( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z );

	return ChunkAtChunkPos( pos.Floor() );
}

block_t *CWorld::BlockAtWorldPos( Vector3f pos )
{
	pos			  = pos.Floor();
	CChunk *chunk = ChunkAtWorldPos( pos );
	if ( chunk == nullptr )
		return nullptr;
	Vector3f localPos = ( pos - chunk->GetPosInWorld() );

	return chunk->GetBlockAtLocal( localPos );
}

void CWorld::SetBlockAtWorldPos( Vector3f pos, BLOCKID block, BLOCKVAL val )
{
	block_t *b = BlockAtWorldPos( pos );
	if ( b == nullptr )
	{ // place it in a buffer
		// smash down to one uint32_t
		uint32_t x			 = block | ( val << 16 );
		m_blocksToPlace[pos] = x;
		return;
	}

	b->Set( block, val );
	b->Update();
}

bool CWorld::ValidChunkPos( const Vector3f pos ) { return ChunkAtWorldPos( pos ) != nullptr; }

void CWorld::AddEntity( CEntityBase *e )
{
	m_ents.push_back( e );
	e->Spawn( this );
}

CEntityBase *CWorld::GetEntityByName( const char *name )
{
	for ( CEntityBase *e : m_ents )
	{
		if ( e->m_name == name )
			return e;
	}
	return nullptr;
}

bool CWorld::TestPointCollision( Vector3f pos )
{
	block_t *b = BlockAtWorldPos( pos );
	if ( b == nullptr )
		return false;
	// BlockFeatures bF = GetBlockFeatures( b->m_iBlockType );
	// if ( !bF.walkable )
	// 	return false;

	if ( !BlockType( b->GetType() ).IsSolid( b->GetMeta() ) )
		return false;

	CBoundingBox blockBounds = BlockType( b->GetType() ).GetBounds();

	pos						= pos - pos.Floor();
	blockBounds.m_vPosition = pos.Floor();
	return blockBounds.TestPointCollide( pos );
}

bool CWorld::TestSelectPointCollision( Vector3f pos )
{
	block_t *b = BlockAtWorldPos( pos );
	if ( b == nullptr )
		return false;
	// BlockFeatures bF = GetBlockFeatures( b->m_iBlockType );
	// if ( !bF.walkable )
	// 	return false;

	if ( !BlockType( b->GetType() ).IsSelectable( b->GetMeta() ) )
		return false;

	CBoundingBox blockBounds = BlockType( b->GetType() ).GetBounds();

	pos						= pos - pos.Floor();
	blockBounds.m_vPosition = pos.Floor();
	return blockBounds.TestPointCollide( pos );
}

block_t *CWorld::TestAABBCollision( CBoundingBox col )
{
	// We can just test all the *blocks* this bounding box intersects.
	// The exact blocks to test is the ceiling of the furthest and floor of the closest
	Vector3f here = col.m_vPosition - col.m_vOrigin * col.m_vBounds;
	Vector3f there = col.m_vPosition + (VEC_ONE - col.m_vOrigin) * col.m_vBounds;
	here = here.Floor();
	there = there.Ceil();

	for (int y = here.y; y < there.y; y++)
	{
		for (int z = here.z; z < there.z; z++)
		{
			for (int x = here.x; x < there.x; x++)
			{
				block_t *block = BlockAtWorldPos( {(float)x, (float)y, (float)z} );
				if (block == nullptr)
					continue;
				
				if ( !BlockType( block->GetType() ).IsSolid( block->GetMeta() ) )
					continue;

				CBoundingBox blockBounds = BlockType( block->GetType() ).GetBounds();
				blockBounds.m_vPosition	 = Vector3f( x, y, z );

				if ( col.TestCollide( blockBounds ) )
					return block;
			}
		}
	}

	return nullptr;
}

void CWorld::WorldTick( int64_t iTick, float delta )
{

	m_ents.erase( std::remove_if( m_ents.begin(), m_ents.end(), []( CEntityBase *e ) { return e->m_bIsKilled; } ),
				  m_ents.end() );

	for ( CEntityBase *ent : m_ents )
	{
		ent->Tick( iTick );
		ent->PhysicsTick( delta, this );
	}

	if ( iTick == m_iLastTick )
		return;
	m_iLastTick = iTick;
	
	// Progress time
	m_iTimeOfDay++;

	if ( m_iTimeOfDay > 24000 )
	{
		m_iTimeOfDay = 0;
	}
}

PortableChunkRepresentation CWorld::GetWorldRepresentation( Vector3f pos )
{
	return GetChunkGenerateAtWorldPos( pos )->m_portableDef;
}

void CWorld::UsePortable( PortableChunkRepresentation rep )
{
	CChunk *c = GetChunkGenerateAtPos( Vector3f( rep.x, rep.y, rep.z ) );

	for ( int j = 0; j < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; j++ )
	{
		c->m_blocks[j].Set( rep.m_iBlocks[j], rep.m_iValue[j] );
	}
}