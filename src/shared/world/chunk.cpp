#include "chunk.hpp"

#include <random>

#include "world.hpp"

#include "logging.hpp"

#ifdef CLIENTEXE
CChunk::CChunk()
{
	for ( int i = 0; i < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; i++ )
	{
		m_iLightingValue[i]		   = 0;
		m_portableDef.m_iBlocks[i] = 0;
		m_portableDef.m_iValue[i]  = 0;

		m_blocks[i].m_pChunk = this;
	}

	m_bDirty = true;
}
CChunk::~CChunk()
{
	// con_debug("deleting chunk @ <%.0f, %.0f, %.0f>", m_vPosition.x, m_vPosition.y, m_vPosition.z);
}
#elif SERVEREXE
CChunk::CChunk()
{
	for ( int i = 0; i < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; i++ )
	{
		m_portableDef.m_iBlocks[i] = 0;
		m_portableDef.m_iValue[i]  = 0;

		m_blocks[i].m_pChunk = this;
	}

	m_bDirty = true;
}
CChunk::~CChunk() {}
#endif

CChunk *CChunk::Neighbour( Direction dir )
{
	Vector3f neighbourPos = m_vPosition + DirectionVector[dir];
	return m_pChunkMan->ChunkAtChunkPos( neighbourPos );
}
CChunk *CChunk::Neighbour( Vector3f dir )
{
	Vector3f neighbourPos = m_vPosition + dir;
	return m_pChunkMan->ChunkAtChunkPos( neighbourPos );
}

#ifdef CLIENTEXE
void CChunk::Render() { m_blocksMdl.Render(); }
void CChunk::RenderTrans() { m_waterMdl.Render(); }

void CChunk::RebuildMdl() { BuildChunkModel( m_blocksMdl, m_waterMdl, m_blocks, GetPosInWorld(), this ); }
#endif

// Takes a coordinate and returns a vector in world coordinates relative to this chunk
// Intended to be used by in-chunk coords but doesn't throw a hissyfit if it's not
Vector3f CChunk::PosToWorld( Vector3f pos ) { return GetPosInWorld() + pos; }

void CChunk::RebuildPortable()
{
	for ( int i = 0; i < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; i++ )
	{
		m_blocks[i].Get(
			m_portableDef.m_iBlocks[i], m_portableDef.m_iValue[i]
		);
	}
}

void CChunk::Update( int64_t iTick )
{
	// If true we're dirty next tick
	bool bDirtyAgain = false;

	if ( iTick == m_iLastTick )
		return;

	m_iLastTick = iTick;

#ifdef SERVEREXE
	// Liquid handling is done in two passes
	// Pass 1 goes over every currently existing block in the chunk
	// If it finds a block marked as liquid, AND the current tick is a multiple
	// of when it would flow, we collect it into a list for pass 2
	// This is done so we don't flow infinitely in one direction because of linear index
	//
	// Pass 2 we go through every block position we intend to process
	// And flow liquid into any blocks that are marked as flowable
	// If we touch a neighbouring chunk, we mark it as dirty
	// If liquid flows in us at all we mark ourselves as dirty for the next tick
	// Where we'll then try flowing again (which might or might not cause another flow)

	// TODO: Re-implement this, but more minecraft-y, and in the new block system

	// std::vector<Vector3f> liquidBlocks = {};
	// for ( int i = 0; i < ( CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z ); i++ )
	// {
	// 	int x, y, z;
	// 	CHUNK1D_TO_3D( i, x, y, z );

	// 	BLOCKID blockType = m_blocks[i].m_iBlockType;

	// 	// Every liquidSpeedth tick
	// 	BlockFeatures bF = GetBlockFeatures( blockType );
	// 	if ( bF.isLiquid && iTick % bF.liquidSpeed == 0 )
	// 	{
	// 		liquidBlocks.push_back( Vector3f( x, y, z ) );
	// 	}
	// }

	// for ( Vector3f pos : liquidBlocks )
	// {
	// 	block_t *blockHandling		= GetBlockAtLocal( pos );
	// 	BLOCKID blockType		= blockHandling->m_iBlockType;
	// 	BlockFeatures blockFeatures = GetBlockFeatures( blockType );

	// 	if ( blockFeatures.isLiquidSource )
	// 		blockHandling->m_iBlockData = blockFeatures.liquidRange;
	// 	else if ( blockHandling->m_iBlockData == 0 )
	// 		continue;

	// 	// Test Bottom first
	// 	block_t *pBlock =  m_pChunkMan->BlockAtWorldPos( PosToWorld( Vector3f( pos.x, pos.y - 1, pos.z ) ) );
	// 	if ( pBlock == nullptr )
	// 		continue;

	// 	BlockFeatures blockF = GetBlockFeatures( pBlock->m_iBlockType );
	// 	if ( blockF.floodable && pBlock->m_iBlockType != blockType )
	// 	{
	// 		pBlock->m_iBlockType = blockFeatures.liquidFlow;
	// 		pBlock->m_iBlockData	 = blockFeatures.liquidRange;
	// 	}
	// 	else if ( pBlock->GetType() == blockFeatures.liquidFlow ||
	// 			  pBlock->GetType() == blockFeatures.liquidSource )
	// 		continue;
	// 	else
	// 	{
	// 		for ( int i = 0; i < 4; i++ )
	// 		{
	// 			Vector3f dir = DirectionVector[i];
	// 			block_t *b	= GetBlockAtLocal( Vector3f( pos.x, pos.y, pos.z ) + dir );
	// 			if ( b == nullptr )
	// 			{
	// 				// It's not in *this* chunk
	// 				CChunk *oChunk = Neighbour( (Direction)i );
	// 				if ( oChunk == nullptr )
	// 					continue; // Ok yeah it's outside reality
	// 				Vector3f p = Vector3f( pos.x + dir.x, pos.y + dir.y, pos.z + dir.z ) +
	// 							( dir * Vector3f( -CHUNKSIZE_X, -CHUNKSIZE_Y, -CHUNKSIZE_Z ) );
	// 				;
	// 				b = oChunk->GetBlockAtLocal( p );
	// 				if ( b == nullptr )
	// 					continue; // uh oh
	// 			}
	// 			BlockFeatures bF = GetBlockFeatures( b->m_iBlockType );
	// 			if ( bF.floodable )
	// 			{
	// 				b->m_iBlockType = blockFeatures.liquidFlow;
	// 				b->m_iBlockData	= blockHandling->m_iBlockData - 1;
	// 				bDirtyAgain		= true; // Something within us changed, we should update next tick too
	// 			}
	// 			else if ( b->GetType() == blockFeatures.liquidFlow )
	// 			{
	// 				if ( b->m_iBlockData < ( blockHandling->m_iBlockData - 1 ) )
	// 					b->m_iBlockData = blockHandling->m_iBlockData - 1;
	// 			}
	// 		}
	// 	}
	// }
#endif

	// Rebuild the portable information at last
	RebuildPortable();

#ifdef CLIENTEXE
	// Chunk update makes neighbours and ourself update our model
	// Hahahahahahah slow
	if ( m_bDirty ) // You're a dirty little purple chunk
	{
		UpdateLighting();
		RebuildMdl();
		for ( int i = 0; i < 6; i++ )
		{
			CChunk *neighbour = Neighbour( (Direction)i );
			if ( neighbour != nullptr )
				neighbour->RebuildMdl();
		}
	}
#endif

	// We updated so we're not dirty
	// We can also assume that if we're dirty again we're *really* dirty and need to resend the whole chunk ASAP!
	m_bDirty	   = bDirtyAgain;
	m_bReallyDirty = bDirtyAgain;
}

block_t *CChunk::GetBlockAtLocal( Vector3f pos )
{
	if ( !ValidChunkPosition( pos ) )
		return nullptr;
	return &m_blocks[int( CHUNK3D_TO_1D( pos.x, pos.y, pos.z ) )];
}

// Returns the block at the given world position, relative to this chunk
block_t *CChunk::GetBlockAtRelative( Vector3f pos )
{
	if (ValidChunkPosition( pos ))
		return GetBlockAtLocal( pos );
	Vector3f thePos = PosToWorld(pos);
	return m_pChunkMan->BlockAtWorldPos( thePos );
}

bool ValidChunkPosition( int x, int y, int z )
{
	// If the position is valid
	return !( x < 0 || y < 0 || z < 0 || x >= CHUNKSIZE_X || y >= CHUNKSIZE_Y || z >= CHUNKSIZE_Z );
}
bool ValidChunkPosition( Vector3f pos )
{
	// If the position is valid
	return ValidChunkPosition( pos.x, pos.y, pos.z );
}

#ifdef CLIENTEXE

CColour CChunk::GetLightingLocal( Vector3f pos )
{
	CColour c = m_iLightingValue[int( CHUNK3D_TO_1D( pos.x, pos.y, pos.z ) )];

	return c;
}

CColour CChunk::GetLightingRelative( Vector3f pos )
{
	if ( ValidChunkPosition( pos ) )
		return GetLightingLocal( pos );
	Vector3f thePos = PosToWorld(pos);
	return m_pChunkMan->GetLightingAtWorldPos( thePos );
}

void CChunk::SetLightingLocal( Vector3f pos, CColour colour )
{
	m_iLightingValue[int( CHUNK3D_TO_1D( pos.x, pos.y, pos.z ) )] = colour;
}

// TODO: Put in the world, not the chunk so it doesn't get weird
// void Zoop( CChunk *c, int r, int g, int b, int s, int x, int y, int z )
// {
// 	if ( c == nullptr )
// 		return; // Run away!

// 	if ( !ValidChunkPosition( x, y, z ) )
// 	{ // Pass the zoopening
// 		int nx, ny, nz = 0;

// 		if ( x >= CHUNKSIZE_X )
// 			nx = 1;
// 		else if ( x < 0 )
// 			nx = -1;
// 		if ( y >= CHUNKSIZE_Y )
// 			ny = 1;
// 		else if ( y < 0 )
// 			ny = -1;
// 		if ( z >= CHUNKSIZE_Z )
// 			nz = 1;
// 		else if ( z < 0 )
// 			nz = -1;

// 		x -= CHUNKSIZE_X * nx;
// 		y -= CHUNKSIZE_Y * ny;
// 		z -= CHUNKSIZE_Z * nz;

// 		Zoop( c->Neighbour( Vector3f( nx, ny, nz ) ), r, g, b, s, x, y, z );

// 		return;
// 	}

// 	int j = CHUNK3D_TO_1D( x, y, z );

// 	BlockFeatures bf = GetBlockFeatures( c->m_blocks[j].m_iBlockType );

// 	// Fully Opaque, doesn't let light through
// 	if ( bf.opaqueness == 0xFFF )
// 		return;

// 	r -= ( bf.opaqueness >> 8 ) & 0xF;
// 	g -= ( bf.opaqueness >> 4 ) & 0xF;
// 	b -= ( bf.opaqueness ) & 0xF;

// 	// TODO: opaqueness S

// 	bool set  = false;
// 	int light = c->m_iLightingValue[j];
// 	if ( r > ( ( light >> 12 ) & 0xF ) )
// 	{
// 		light = ( light & 0x0FFF ) | ( r << 12 );
// 		set	  = true;
// 	}
// 	if ( g > ( ( light >> 8 ) & 0xF ) )
// 	{
// 		light = ( light & 0xF0FF ) | ( g << 8 );
// 		set	  = true;
// 	}
// 	if ( b > ( ( light >> 4 ) & 0xF ) )
// 	{
// 		light = ( light & 0xFF0F ) | ( b << 4 );
// 		set	  = true;
// 	}
// 	c->m_iLightingValue[j] = light;

// 	if ( !set )
// 		return;

// 	r--;
// 	r = r > 0 ? r : 0;
// 	g--;
// 	g = g > 0 ? g : 0;
// 	b--;
// 	b = b > 0 ? b : 0;
// 	s--;
// 	s = s > 0 ? s : 0;

// 	if ( r <= 0 && g <= 0 && b <= 0 && s <= 0 )
// 		return;

// 	Zoop( c, r, g, b, s, x + 1, y, z );
// 	Zoop( c, r, g, b, s, x - 1, y, z );
// 	Zoop( c, r, g, b, s, x, y + 1, z );
// 	Zoop( c, r, g, b, s, x, y - 1, z );
// 	Zoop( c, r, g, b, s, x, y, z + 1 );
// 	Zoop( c, r, g, b, s, x, y, z - 1 );
// }

// TODO: Put in the world, not the chunk so it doesn't get weird
void CChunk::UpdateLighting()
{
	// TODO: sunlight
	// Zero out & reset
	for ( int i = 0; i < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; i++ )
		m_iLightingValue[i] = 0xFFFF;

	// for ( int i = 0; i < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; i++ )
	// {
	// 	BlockFeatures bF = GetBlockFeatures( m_blocks[i].m_iBlockType );

	// 	if ( bF.isLightSource )
	// 	{
	// 		int x, y, z;
	// 		CHUNK1D_TO_3D( i, x, y, z );
	// 		Zoop( this, ( bF.lightColour >> 8 ) & 0xF, ( bF.lightColour >> 4 ) & 0xF, ( bF.lightColour ) & 0xF, 0, x, y,
	// 			  z );
	// 	}
	// }
}

#endif