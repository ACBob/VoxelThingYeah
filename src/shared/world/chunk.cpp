#include "chunk.hpp"

#include <random>

#include "world.hpp"

#ifdef CLIENTEXE
CChunk::CChunk()
{
	for ( int i = 0; i < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; i++ )
	{
		m_blocks[i].m_iBlockType   = blocktype_t::AIR;
		m_iLightingValue[i]		   = 0x00000000;
		m_portableDef.m_iBlocks[i] = m_blocks[i].m_iBlockType;
		m_portableDef.m_iVal[i] = m_blocks[i].m_iValueA;
		m_portableDef.m_iValB[i] = m_blocks[i].m_iValueB;
	}

	m_bDirty = true;
}
CChunk::~CChunk() {}
#elif SERVEREXE
CChunk::CChunk()
{
	for ( int i = 0; i < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; i++ )
	{
		m_blocks[i].m_iBlockType   = blocktype_t::AIR;
		m_portableDef.m_iBlocks[i] = m_blocks[i].m_iBlockType;
		m_portableDef.m_iVal[i] = m_blocks[i].m_iValueA;
		m_portableDef.m_iValB[i] = m_blocks[i].m_iValueB;
	}

	m_bDirty = true;
}
CChunk::~CChunk() {}
#endif

CChunk *CChunk::Neighbour( Direction dir )
{
	CVector neighbourPos = m_vPosition + DirectionVector[dir];
	return ( reinterpret_cast<CWorld *>( m_pChunkMan ) )->ChunkAtChunkPos( neighbourPos );
}

#ifdef CLIENTEXE
void CChunk::Render() { m_blocksMdl.Render(); }
void CChunk::RenderTrans() { m_waterMdl.Render(); }

void CChunk::RebuildMdl() { BuildChunkModel( m_blocksMdl, m_waterMdl, m_blocks, GetPosInWorld(), this ); }
#endif

// Takes a coordinate and returns a vector in world coordinates relative to this chunk
// Intended to be used by in-chunk coords but doesn't throw a hissyfit if it's not
CVector CChunk::PosToWorld( CVector pos ) { return GetPosInWorld() + pos; }

void CChunk::Update( int64_t iTick )
{
	// If true we're dirty next tick
	bool bDirtyAgain = false;

	if (iTick == m_iLastTick)
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

	std::vector<CVector> liquidBlocks = {};
	for ( int i = 0; i < ( CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z ); i++ )
	{
		int x, y, z;
		CHUNK1D_TO_3D( i, x, y, z );

		blocktype_t blockType = m_blocks[i].m_iBlockType;

		// Every liquidSpeedth tick
		BlockFeatures bF = GetBlockFeatures( blockType );
		if ( bF.isLiquid && iTick % bF.liquidSpeed == 0 )
		{
			liquidBlocks.push_back( CVector( x, y, z ) );
		}
	}

	for ( CVector pos : liquidBlocks )
	{
		CBlock *blockHandling = GetBlockAtLocal( pos );
		blocktype_t blockType = blockHandling->m_iBlockType;
		BlockFeatures blockFeatures = GetBlockFeatures(blockType);

		if (blockFeatures.isLiquidSource)
			blockHandling->m_iValueA = blockFeatures.liquidRange;
		else if (blockHandling->m_iValueA == 0)
			continue;

		// Test Bottom first
		CBlock *pBlock = reinterpret_cast<CWorld *>( m_pChunkMan )
							 ->BlockAtWorldPos( PosToWorld( CVector( pos.x, pos.y - 1, pos.z ) ) );
		if ( pBlock == nullptr )
			continue;

		BlockFeatures blockF = GetBlockFeatures( pBlock->m_iBlockType );
		if ( blockF.floodable && pBlock->m_iBlockType != blockType )
		{
			pBlock->m_iBlockType = blockFeatures.liquidFlow;
			pBlock->m_iValueA = blockHandling->m_iValueA;
		}
		else if ( pBlock->m_iBlockType == blockFeatures.liquidFlow || pBlock->m_iBlockType == blockFeatures.liquidSource )
			continue;
		else
		{
			for ( int i = 0; i < 4; i++ )
			{
				CVector dir = DirectionVector[i];
				CBlock *b	= GetBlockAtLocal( CVector( pos.x, pos.y, pos.z ) + dir );
				if ( b == nullptr )
				{
					// It's not in *this* chunk
					CChunk *oChunk = Neighbour( (Direction)i );
					if ( oChunk == nullptr )
						continue; // Ok yeah it's outside reality
					CVector p = CVector( pos.x + dir.x, pos.y + dir.y, pos.z + dir.z ) +
								( dir * CVector( -CHUNKSIZE_X, -CHUNKSIZE_Y, -CHUNKSIZE_Z ) );
					;
					b = oChunk->GetBlockAtLocal( p );
					if ( b == nullptr )
						continue; // uh oh
				}
				BlockFeatures bF = GetBlockFeatures( b->m_iBlockType );
				if ( bF.floodable )
				{
					b->m_iBlockType = blockFeatures.liquidFlow;
					b->m_iValueA = blockHandling->m_iValueA - 1;
					bDirtyAgain		= true; // Something within us changed, we should update next tick too
				}
			}
		}
	}

	// Rebuild the portable information at last
	m_portableDef.x = m_vPosition.x;
	m_portableDef.y = m_vPosition.y;
	m_portableDef.z = m_vPosition.z;

	for ( int j = 0; j < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; j++ )
	{
		m_portableDef.m_iBlocks[j] = m_blocks[j].m_iBlockType;
		m_portableDef.m_iVal[j] = m_blocks[j].m_iValueA;
		m_portableDef.m_iValB[j] = m_blocks[j].m_iValueB;
	}
#endif

#ifdef CLIENTEXE
	// Chunk update makes neighbours and ourself update our model
	// Hahahahahahah slow
	if (m_bDirty) // You're a dirty little purple chunk
	{
		RebuildMdl();
		for ( int i = 0; i < 6; i++ )
		{
			CChunk *neighbour = Neighbour( (Direction)i );
			if ( neighbour != nullptr )
				neighbour->RebuildMdl();
		}
		// TODO: Lighting
	}
#endif

	// We updated so we're not dirty
	m_bDirty	   = bDirtyAgain;
}

CBlock *CChunk::GetBlockAtLocal( CVector pos )
{
	if ( !ValidChunkPosition( pos ) )
		return nullptr;
	return &m_blocks[int( CHUNK3D_TO_1D( pos.x, pos.y, pos.z ) )];
}

bool ValidChunkPosition( CVector pos )
{
	// If the position is valid
	return !( pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= CHUNKSIZE_X || pos.y >= CHUNKSIZE_Y ||
			  pos.z >= CHUNKSIZE_Z );
}

#ifdef CLIENTEXE

Colour CChunk::GetLightingLocal( CVector pos )
{
	Colour c;
	uint16_t l = m_iLightingValue[int( CHUNK3D_TO_1D( pos.x, pos.y, pos.z ) )];
	c.x		   = ( l >> 8 ) & 0xF;
	c.y		   = ( l >> 4 ) & 0xF;
	c.z		   = l & 0xF;
	c.w		   = ( l >> 16 ) & 0xF;

	return c;
}

void CChunk::SetLightingLocal( CVector pos, Colour colour )
{
	uint16_t l = m_iLightingValue[int( CHUNK3D_TO_1D( pos.x, pos.y, pos.z ) )];
	l		   = ( l & 0x0FFF ) | ( (int)colour.w << 16 );
	l		   = ( l & 0xF0FF ) | ( (int)colour.x << 8 );
	l		   = ( l & 0xFF0F ) | ( (int)colour.x << 4 );
	l		   = ( l & 0xFFF0 ) | ( (int)colour.x );
}

#endif