#include "world.hpp"

#include "entities/entitybase.hpp"

#include "physics.hpp"

#include "shared/seethe.h"

#ifdef SERVEREXE
	#include "server/cvar_serverside.hpp"
#endif

#ifdef CLIENTEXE
CWorld::CWorld( CShader *shader, CShader *entShader ) : worldShader( shader ), entityShader( entShader )
#elif SERVEREXE
CWorld::CWorld()
#endif
{
	chunks = {};

	// Now that all the chunks exist, generate and rebuild their models
	for ( CChunk *c : chunks )
	{
		jenerator.Generate( c );
#ifdef CLIENTEXE
		c->RebuildMdl();
#endif
	}
}
CWorld::~CWorld()
{
	// Destroy chunks
	for ( CChunk *c : chunks )
		delete c;
}

// Return in good faith that it's a valid position
CChunk *CWorld::ChunkAtChunkPos( CVector pos )
{
	for ( CChunk *c : chunks )
		if ( c->position == pos )
			return c;

	return nullptr;
}

// Tries to get a chunk and generates a new one if it can't find one
CChunk *CWorld::GetChunkGenerateAtWorldPos( CVector pos )
{
	CChunk *c = ChunkAtWorldPos( pos );
	if ( c != nullptr )
		return c;

	c			= new CChunk();
	c->position = ( pos / CVector( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ).Floor();
	c->chunkMan = this;
#ifdef CLIENTEXE
	c->mdl.position = c->GetPosInWorld();
	c->mdl.SetShader( worldShader );
#endif
	jenerator.Generate( c );

	chunks.push_back( c );

	return c;
}

void CWorld::UnloadChunk( CVector pos )
{
	CChunk *c = ChunkAtChunkPos( pos );
	if ( c == nullptr )
		return;

	for ( auto i = chunks.begin(); i != chunks.end(); ++i )
	{
		if ( *i == c )
		{
			chunks.erase( i );
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
	pos			 = pos.Floor();
	CChunk *chunk = ChunkAtWorldPos( pos );
	if ( chunk == nullptr )
		return nullptr;
	CVector localPos = ( pos - chunk->GetPosInWorld() );

	return chunk->GetBlockAtLocal( localPos );
}

bool CWorld::ValidChunkPos( const CVector pos ) { return ChunkAtWorldPos( pos ) != nullptr; }

void CWorld::AddEntity( void *e )
{
	ents.push_back( e );
	( (CEntityBase *)e )->Spawn();
#ifdef CLIENTEXE
	( (CEntityBase *)e )->SetShader( entityShader );
#endif
}

void *CWorld::GetEntityByName( const char *name )
{
	for ( void *e : ents )
	{
		CEntityBase *ent = reinterpret_cast<CEntityBase *>( e );
		if ( ent->name == name )
			return e;
	}
	return nullptr;
}

#ifdef CLIENTEXE
void CWorld::Render()
{
	for ( CChunk *c : chunks )
	{
		c->Render();
	}
	for ( void *ent : ents )
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
	BlockFeatures bF = GetBlockFeatures( b->blockType );
	if ( !bF.walkable )
		return false;

	pos = pos - pos.Floor();
	return CBoundingBox( pos.Floor(), CVector( 1, 1, 1 ), CVector( 0 ) ).TestPointCollide( pos );
}

bool CWorld::TestAABBCollision( CBoundingBox col )
{
	CChunk *chunk = ChunkAtWorldPos( col.pos );
	if ( chunk == nullptr )
		return false;

	// Test A (pos)
	for ( int i = 0; i < ( CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z ); i++ )
	{
		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );

		// Don't collide with air
		blocktype_t blockType = chunk->blocks[i].blockType;
		BlockFeatures bF	  = GetBlockFeatures( blockType );
		if ( !bF.walkable )
			continue;

		if ( col.TestCollide( CBoundingBox( chunk->GetPosInWorld() + CVector( x, y, z ), CVector( 1, 1, 1 ), CVector( 0 ) ) ) )
			return true;
	}

	return false;
}

void CWorld::WorldTick( int tickN )
{
	for ( int i = 0; i < ents.size(); i++ )
	{
		void *ent = ents[i];

		if ( reinterpret_cast<CEntityBase *>( ent )->isKilled )
		{
			// Clear from world
			con_debug( "removing entity" );
			ents.erase( ents.begin() + i );

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
	for ( CChunk *chunk : chunks )
	{
		// Generally we should avoid rebuilding the universe every tick
		bool rebuild					 = false;
		std::vector<CVector> liquidBlocks = {};
		for ( int i = 0; i < ( CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z ); i++ )
		{
			int x, y, z;
			CHUNK1D_TO_3D( i, x, y, z );

			blocktype_t blockType = chunk->blocks[i].blockType;

			// Every liquidSpeedth tick
			BlockFeatures bF = GetBlockFeatures( blockType );
			if ( bF.isLiquid && tickN % bF.liquidSpeed == 0 )
			{
				liquidBlocks.push_back( CVector( x, y, z ) );
			}
		}

		for ( CVector pos : liquidBlocks )
		{
			blocktype_t blockType = chunk->GetBlockAtLocal( pos )->blockType;
			for ( int i = 0; i < 5; i++ )
			{
				if ( i == UP )
					i = DOWN;

				CVector dir = DirectionVector[i];
				CBlock *b   = chunk->GetBlockAtLocal( CVector( pos.x, pos.y, pos.z ) + dir );
				if ( b == nullptr )
				{
					// It's not in *this* chunk
					CChunk *oChunk = chunk->Neighbour( (Direction)i );
					if ( oChunk == nullptr )
						continue; // Ok yeah it's outside reality
					CVector p = CVector( pos.x + dir.x, pos.y + dir.y, pos.z + dir.z ) + ( dir * -16 );
					b		 = oChunk->GetBlockAtLocal( p );
					if ( b == nullptr )
						continue; // uh oh
				}
				BlockFeatures bF = GetBlockFeatures( b->blockType );
				if ( bF.floodable )
				{
					b->blockType = blockType;
					rebuild		 = true;
				}
			}
		}

		if ( rebuild )
			chunk->Update();
	}

	// Progress time
	timeOfDay++;
#endif
	if ( timeOfDay > 24000 )
	{
		timeOfDay = 0;
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
	crep.x	 = c->position.x;
	crep.y	 = c->position.y;
	crep.z	 = c->position.z;

	for ( int j = 0; j < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; j++ )
	{
		crep.blocks[j] = c->blocks[j].blockType;
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
		c->blocks[j].blockType = (blocktype_t)rep.blocks[j];
	}

	c->Update();
}