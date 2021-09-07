#include "world.hpp"

#include "entities/entitybase.hpp"

#include "physics.hpp"

#include "shared/seethe.h"

#ifdef SERVEREXE
	#include "server/cvar_serverside.hpp"
#endif

#ifdef CLIENTEXE
CWorld::CWorld( CShader *shader, CShader *entShader ) : m_pWorldShader( shader ), m_pEntityShader( entShader )
#elif SERVEREXE
CWorld::CWorld()
#endif
{
	m_chunks = {};

	// Now that all the chunks exist, generate and rebuild their models
	for ( CChunk *c : m_chunks )
	{
		m_jenerator.Generate( c );
#ifdef CLIENTEXE
		c->RebuildMdl();
#endif
	}
}
CWorld::~CWorld()
{
	// Destroy chunks
	for ( CChunk *c : m_chunks )
		delete c;
}

// Return in good faith that it's a valid position
CChunk *CWorld::ChunkAtChunkPos( CVector pos )
{
	for ( CChunk *c : m_chunks )
		if ( c->m_vPosition == pos )
			return c;

	return nullptr;
}

// Tries to get a chunk and generates a new one if it can't find one
CChunk *CWorld::GetChunkGenerateAtWorldPos( CVector pos )
{
	CChunk *c = ChunkAtWorldPos( pos );
	if ( c != nullptr )
		return c;

	c			   = new CChunk();
	c->m_vPosition = ( pos / CVector( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ).Floor();
	c->m_pChunkMan = this;
#ifdef CLIENTEXE
	c->m_mdl.m_vPosition = c->GetPosInWorld();
	c->m_mdl.SetShader( m_pWorldShader );
#endif
	m_jenerator.Generate( c );

	m_chunks.push_back( c );

	return c;
}

void CWorld::UnloadChunk( CVector pos )
{
	CChunk *c = ChunkAtChunkPos( pos );
	if ( c == nullptr )
		return;

	for ( auto i = m_chunks.begin(); i != m_chunks.end(); ++i )
	{
		if ( *i == c )
		{
			m_chunks.erase( i );
			return;
		}
	}
}

// Return in good faith that it's a valid position
CChunk *CWorld::ChunkAtWorldPos( CVector pos )
{
	pos = pos / CVector( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z );
	pos = pos.Floor();

	return ChunkAtChunkPos( pos );
}

CBlock *CWorld::BlockAtWorldPos( CVector pos )
{
	pos			  = pos.Floor();
	CChunk *chunk = ChunkAtWorldPos( pos );
	if ( chunk == nullptr )
		return nullptr;
	CVector localPos = ( pos - chunk->GetPosInWorld() );

	return chunk->GetBlockAtLocal( localPos );
}

bool CWorld::ValidChunkPos( const CVector pos ) { return ChunkAtWorldPos( pos ) != nullptr; }

void CWorld::AddEntity( void *e )
{
	m_ents.push_back( e );
	( (CEntityBase *)e )->Spawn();
#ifdef CLIENTEXE
	( (CEntityBase *)e )->SetShader( m_pEntityShader );
#endif
}

void *CWorld::GetEntityByName( const char *name )
{
	for ( void *e : m_ents )
	{
		CEntityBase *ent = reinterpret_cast<CEntityBase *>( e );
		if ( ent->m_name == name )
			return e;
	}
	return nullptr;
}

#ifdef CLIENTEXE
void CWorld::Render()
{
	for ( CChunk *c : m_chunks )
	{
		c->Render();
	}
	for ( void *ent : m_ents )
	{
		( (CEntityBase *)ent )->Render();
	}
}
#endif

bool CWorld::TestPointCollision( CVector pos )
{
	CBlock *b = BlockAtWorldPos( pos );
	if ( b == nullptr )
		return false;
	BlockFeatures bF = GetBlockFeatures( b->m_iBlockType );
	if ( !bF.walkable )
		return false;

	pos = pos - pos.Floor();
	return CBoundingBox( pos.Floor(), CVector( 1, 1, 1 ), CVector( 0 ) ).TestPointCollide( pos );
}

bool CWorld::TestAABBCollision( CBoundingBox col )
{
	CChunk *chunk = ChunkAtWorldPos( col.m_vPosition );
	if ( chunk == nullptr )
		return false;

	// Test A (pos)
	for ( int i = 0; i < ( CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z ); i++ )
	{
		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );

		// Don't collide with air
		blocktype_t blockType = chunk->m_blocks[i].m_iBlockType;
		BlockFeatures bF	  = GetBlockFeatures( blockType );
		if ( !bF.walkable )
			continue;

		if ( col.TestCollide(
				 CBoundingBox( chunk->GetPosInWorld() + CVector( x, y, z ), CVector( 1, 1, 1 ), CVector( 0 ) ) ) )
			return true;
	}

	return false;
}

void CWorld::WorldTick( int tickN )
{
	for ( int i = 0; i < m_ents.size(); i++ )
	{
		void *ent = m_ents[i];

		if ( reinterpret_cast<CEntityBase *>( ent )->m_bIsKilled )
		{
			// Clear from world
			con_debug( "removing entity" );
			m_ents.erase( m_ents.begin() + i );

			continue;
		}
		reinterpret_cast<CEntityBase *>( ent )->Tick();
#ifdef SERVEREXE
		reinterpret_cast<CEntityBase *>( ent )->PhysicsTick( sv_tickms->GetInt() / 1000.0f, this );
#else
		// TODO: get tick speed from server
		reinterpret_cast<CEntityBase *>( ent )->PhysicsTick( 0.05, this );
#endif
	}

#ifdef SERVEREXE
	for ( CChunk *chunk : m_chunks )
	{
		// Generally we should avoid rebuilding the universe every tick
		bool rebuild					  = false;
		std::vector<CVector> liquidBlocks = {};
		for ( int i = 0; i < ( CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z ); i++ )
		{
			int x, y, z;
			CHUNK1D_TO_3D( i, x, y, z );

			blocktype_t blockType = chunk->m_blocks[i].m_iBlockType;

			// Every liquidSpeedth tick
			BlockFeatures bF = GetBlockFeatures( blockType );
			if ( bF.isLiquid && tickN % bF.liquidSpeed == 0 )
			{
				liquidBlocks.push_back( CVector( x, y, z ) );
			}
		}

		for ( CVector pos : liquidBlocks )
		{
			blocktype_t blockType = chunk->GetBlockAtLocal( pos )->m_iBlockType;
			for ( int i = 0; i < 5; i++ )
			{
				if ( i == UP )
					i = DOWN;

				CVector dir = DirectionVector[i];
				CBlock *b	= chunk->GetBlockAtLocal( CVector( pos.x, pos.y, pos.z ) + dir );
				if ( b == nullptr )
				{
					// It's not in *this* chunk
					CChunk *oChunk = chunk->Neighbour( (Direction)i );
					if ( oChunk == nullptr )
						continue; // Ok yeah it's outside reality
					CVector p = CVector( pos.x + dir.x, pos.y + dir.y, pos.z + dir.z ) + ( dir * -16 );
					b		  = oChunk->GetBlockAtLocal( p );
					if ( b == nullptr )
						continue; // uh oh
				}
				BlockFeatures bF = GetBlockFeatures( b->m_iBlockType );
				if ( bF.floodable )
				{
					b->m_iBlockType = blockType;
					rebuild			= true;
				}
			}
		}

		if ( rebuild )
			chunk->Update();
	}

	// Progress time
	m_iTimeOfDay++;
#endif
	if ( m_iTimeOfDay > 24000 )
	{
		m_iTimeOfDay = 0;
	}
}

CWorld::PortableChunkRepresentation CWorld::GetWorldRepresentation( CVector pos )
{
	PortableChunkRepresentation crep;

	// if (!ValidChunkPos(pos))
	// {
	// 	con_critical("We've just returned garbage data for the portable chunk!");
	// 	return crep;
	// }

	CChunk *c = GetChunkGenerateAtWorldPos( pos * CVector( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) );
	crep.x	  = c->m_vPosition.x;
	crep.y	  = c->m_vPosition.y;
	crep.z	  = c->m_vPosition.z;

	for ( int j = 0; j < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; j++ )
	{
		crep.blocks[j] = c->m_blocks[j].m_iBlockType;
	}

	return crep;
}

void CWorld::UsePortable( PortableChunkRepresentation rep )
{
	CChunk *c = GetChunkGenerateAtWorldPos( CVector( rep.x * CHUNKSIZE_X, rep.y * CHUNKSIZE_Y, rep.z * CHUNKSIZE_Z ) );
	if ( c == nullptr )
	{
		con_error( "WEE WOO WEE WOO" );
		return;
	}

	for ( int j = 0; j < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; j++ )
	{
		c->m_blocks[j].m_iBlockType = (blocktype_t)rep.blocks[j];
	}

	c->Update();
}