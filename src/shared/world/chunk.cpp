#include "chunk.hpp"

#include <random>

#include "world.hpp"

#include "logging.hpp"

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

CChunk *CChunk::Neighbour( Direction dir )
{
	Vector3i neighbourPos = m_vPosition + DirectionVector[dir];
	return m_pChunkMan->ChunkAtChunkPos( neighbourPos );
}
CChunk *CChunk::Neighbour( Vector3i dir )
{
	Vector3i neighbourPos = m_vPosition + dir;
	return m_pChunkMan->ChunkAtChunkPos( neighbourPos );
}

// Takes a coordinate and returns a vector in world coordinates relative to this chunk
// Intended to be used by in-chunk coords but doesn't throw a hissyfit if it's not
Vector3i CChunk::PosToWorld( Vector3i pos ) { return GetPosInWorld() + pos; }
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

	// Rebuild the portable information at last
	RebuildPortable();

	// We updated so we're not dirty
	// We can also assume that if we're dirty again we're *really* dirty and need to resend the whole chunk ASAP!
	m_bDirty	   = bDirtyAgain;
	m_bReallyDirty = bDirtyAgain;
}

block_t *CChunk::GetBlockAtLocal( Vector3i pos )
{
	if ( !ValidChunkPosition( pos ) )
		return nullptr;
	return &m_blocks[int( CHUNK3D_TO_1D( pos.x, pos.y, pos.z ) )];
}

// Returns the block at the given world position, relative to this chunk
block_t *CChunk::GetBlockAtRelative( Vector3i pos )
{
	if (ValidChunkPosition( pos ))
		return GetBlockAtLocal( pos );
	Vector3i thePos = PosToWorld(pos);
	return m_pChunkMan->BlockAtWorldPos( thePos );
}

bool ValidChunkPosition( int x, int y, int z )
{
	// If the position is valid
	return !( x < 0 || y < 0 || z < 0 || x >= CHUNKSIZE_X || y >= CHUNKSIZE_Y || z >= CHUNKSIZE_Z );
}
bool ValidChunkPosition( Vector3i pos )
{
	// If the position is valid
	return ValidChunkPosition( pos.x, pos.y, pos.z );
}