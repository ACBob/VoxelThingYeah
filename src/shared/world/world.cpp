#include "world.hpp"

#include "entities/entitybase.hpp"

#include "physics.hpp"

#include "shared/logging.hpp"

#ifdef SERVEREXE
	#include "server/cvar_serverside.hpp"
#elif CLIENTEXE
	#include "client/cvar_clientside.hpp"
#endif

#include <algorithm>

#include "blocks/blockbase.hpp"

#ifdef CLIENTEXE
CWorld::CWorld( CShader *shader, CShader *entShader, CShader *waterShader, CTexture *worldTex )
	: m_pWorldShader( shader ), m_pEntityShader( entShader ), m_pWaterShader( waterShader ), m_pWorldTex( worldTex )
#elif SERVEREXE
CWorld::CWorld()
#endif
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
	return GetChunkGenerateAtPos( (pos / Vector3f(CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z)).Floor() );
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
#ifdef CLIENTEXE
	c->m_blocksMdl.m_vPosition = c->GetPosInWorld();
	c->m_blocksMdl.SetShader( m_pWorldShader );
	c->m_blocksMdl.SetTexture( m_pWorldTex );
	c->m_waterMdl.m_vPosition = c->GetPosInWorld();
	c->m_waterMdl.SetShader( m_pWaterShader );
	c->m_waterMdl.SetTexture( m_pWorldTex );
#elif SERVEREXE
	// Generate only on the server
	m_jenerator.Generate( c );
#endif

	// test if there's any buffer blocks that intersect with this chunk
	for ( auto &&b : m_blocksToPlace )
	{
		if ( b.first.x >= c->m_vPosition.x * CHUNKSIZE_X && b.first.x < (c->m_vPosition.x + 1) * CHUNKSIZE_X &&
			 b.first.y >= c->m_vPosition.y * CHUNKSIZE_Y && b.first.y < (c->m_vPosition.y + 1) * CHUNKSIZE_Y &&
			 b.first.z >= c->m_vPosition.z * CHUNKSIZE_Z && b.first.z < (c->m_vPosition.z + 1) * CHUNKSIZE_Z )
		{
			CBlock *blck = c->GetBlockAtLocal( b.first );
			if ( blck == nullptr )
				continue; // (What?)
			// b.second is a 32bit int, and the block type is a 16bit int
			blck->m_iBlockType = (BLOCKID)(b.second & 0xFFFF);
			blck->m_iBlockData = (b.second >> 16) & 0xFFFF;
		}
	}

	// TODO: remove the buffer blocks that intersect with this chunk
	// Using std::remove_if is a bit of a pain, but it's the only way I can think of to do this
	// m_blocksToPlace.erase(
	// 	std::remove_if( m_blocksToPlace.begin(), m_blocksToPlace.end(),
	// 					[c]( const std::pair<Vector3i, uint32_t> &b ) {
	// 						return b.first.x >= c->m_vPosition.x * CHUNKSIZE_X && b.first.x < (c->m_vPosition.x + 1) * CHUNKSIZE_X &&
	// 							   b.first.y >= c->m_vPosition.y * CHUNKSIZE_Y && b.first.y < (c->m_vPosition.y + 1) * CHUNKSIZE_Y &&
	// 							   b.first.z >= c->m_vPosition.z * CHUNKSIZE_Z && b.first.z < (c->m_vPosition.z + 1) * CHUNKSIZE_Z;
	// 					} ),
	// 	m_blocksToPlace.end() );

	c->m_bReallyDirty = c->m_bDirty = true;

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

CBlock *CWorld::BlockAtWorldPos( Vector3f pos )
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
	CBlock *b = BlockAtWorldPos( pos );
	if ( b == nullptr ) { // place it in a buffer
		// smash down to one uint32_t
		uint32_t x = block | ( val << 16 );
		m_blocksToPlace[pos] = x;
		return;
	}

	b->m_iBlockType		= block;
	b->m_iBlockData		= val;
	b->Update();
}

#ifdef CLIENTEXE
CColour CWorld::GetLightingAtWorldPos( Vector3f pos )
{
	pos			  = pos.Floor();
	CChunk *chunk = ChunkAtWorldPos( pos );
	if ( chunk == nullptr )
		return CColour( 0, 0, 0 );
	Vector3f localPos = ( pos - chunk->GetPosInWorld() );

	return chunk->GetLightingLocal( localPos );
}
#endif

bool CWorld::ValidChunkPos( const Vector3f pos ) { return ChunkAtWorldPos( pos ) != nullptr; }

void CWorld::AddEntity( CEntityBase *e )
{
	m_ents.push_back( e );
	e->Spawn( this );
#ifdef CLIENTEXE
	e->SetShader( m_pEntityShader );
#endif
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

#ifdef CLIENTEXE
void CWorld::Render()
{
	// Render regular blocks
	for ( auto &&c : m_chunks )
		c.get()->Render();
	// Render entities
	for ( CEntityBase *ent : m_ents )
	{
		ent->Render();
	}
	// Render stuff like water
	for ( auto &&c : m_chunks )
		c.get()->RenderTrans();
}
#endif

bool CWorld::TestPointCollision( Vector3f pos )
{
	CBlock *b = BlockAtWorldPos( pos );
	if ( b == nullptr )
		return false;
	// BlockFeatures bF = GetBlockFeatures( b->m_iBlockType );
	// if ( !bF.walkable )
	// 	return false;

	if ( !BlockType( b->m_iBlockType ).IsSolid( b->m_iBlockData ) )
		return false;

	CBoundingBox blockBounds = BlockType( b->m_iBlockType ).GetBounds();

	pos						= pos - pos.Floor();
	blockBounds.m_vPosition = pos.Floor();
	return blockBounds.TestPointCollide( pos );
}

CBlock *CWorld::TestAABBCollision( CBoundingBox col )
{
	CChunk *chunk = ChunkAtWorldPos( col.m_vPosition );
	if ( chunk == nullptr )
		return nullptr;

	// Test A (pos)
	for ( int i = 0; i < ( CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z ); i++ )
	{
		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );

		// Don't collide with air
		BLOCKID blockType = chunk->m_blocks[i].m_iBlockType;

		if ( !BlockType( blockType ).IsSolid( chunk->m_blocks[i].m_iBlockData ) )
			continue;

		CBoundingBox blockBounds = BlockType( blockType ).GetBounds();
		blockBounds.m_vPosition	 = chunk->GetPosInWorld() + Vector3f( x, y, z );

		if ( col.TestCollide( blockBounds ) )
			return &chunk->m_blocks[i];
	}

	return nullptr;
}

void CWorld::WorldTick( int64_t iTick, float delta )
{
	std::vector<Vector3f> playerPositions;

	m_ents.erase(
		std::remove_if( m_ents.begin(), m_ents.end(), []( CEntityBase *e ) { return e->m_bIsKilled; } ),
		m_ents.end() );

	for ( CEntityBase *ent : m_ents )
	{
		ent->Tick( iTick );
		ent->PhysicsTick( delta, this );

		if ( ent->IsPlayer() )
			playerPositions.push_back( ( ent->m_vPosition /
										 Vector3f( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) )
										   .Floor() );
	}

	if ( iTick == m_iLastTick )
		return;
	m_iLastTick = iTick;

	for ( auto &&c : m_chunks )
	{
		if ( c->m_bDirty || c->m_bReallyDirty )
		{
			c->Update( iTick );
			continue;
		}

		for ( Vector3f plyrPos : playerPositions )
		{
			if ( c->m_vPosition.Distance(plyrPos) < 7 )
			{
				c->Update( iTick );
				break;
			}
		}
	}

	// Progress time
#ifdef CLIENTEXE
	if ( cl_dodaylightcycle->GetBool() )
#endif
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
		c->m_blocks[j].m_iBlockType = (BLOCKID)rep.m_iBlocks[j];
		c->m_blocks[j].m_iBlockData = rep.m_iValue[j];
	}
}