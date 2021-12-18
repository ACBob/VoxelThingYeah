#include "utility/vector.hpp"

#include "world/block.hpp"

#include "utility/fastnoise.h"

#define CHUNKSIZE_X 16
#define CHUNKSIZE_Y 16
#define CHUNKSIZE_Z 16

// Why yes, I am a C++ Programmer, how could you tell?
#define CHUNK3D_TO_1D( x, y, z ) x + y *CHUNKSIZE_X + z *CHUNKSIZE_X *CHUNKSIZE_Z
#define CHUNK1D_TO_3D( i, x, y, z )                                                                                    \
	z = round( i / ( CHUNKSIZE_X * CHUNKSIZE_Y ) );                                                                    \
	y = round( ( i - z * CHUNKSIZE_X * CHUNKSIZE_Y ) / CHUNKSIZE_X );                                                  \
	x = i - CHUNKSIZE_X * ( y + CHUNKSIZE_Y * z )

#pragma once

// A storage only type of chunk
// that stores JUST essential information to rebuild a chunk
// Used for saving/loading and network stuff
struct PortableChunkRepresentation
{
	// Vector3i can't be used in this context
	int32_t x, y, z;
	BLOCKVAL m_iValue[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];
	BLOCKID m_iBlocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];
};

// Forward declaration
class CWorld;

class CChunk
{
  public:
	CChunk();
	~CChunk();

	CChunk *Neighbour( Direction dir );
	CChunk *Neighbour( Vector3i dir );

	Vector3i m_vPosition;
	Vector3i GetPosInWorld() { return m_vPosition * Vector3i( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ); }
	Vector3i GetPosInWorld( Vector3i pos )
	{
		return ( m_vPosition * Vector3i( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ) + pos;
	}

	block_t *GetBlockAtLocal( Vector3i pos );

	block_t *GetBlockAtRelative( Vector3i pos );

	void Update( int64_t iTick );

	Vector3i PosToWorld( int x, int y, int z );
	Vector3i PosToWorld( Vector3i pos );
	Vector3f PosToWorld( Vector3f pos );

	void RebuildPortable();

	// Flat array of blocks, access with
	// Indexed with [x + SIZEX * (y + SIZEZ * z)]
	block_t m_blocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];

	CWorld *m_pChunkMan = nullptr;

	PortableChunkRepresentation m_portableDef;

	bool m_bDirty		= false; // We need to update regardless of if we're near anyone
	bool m_bReallyDirty = false; // We need to be completely resent

	int64_t m_iLastTick = 0;
};

bool ValidChunkPosition( Vector3i pos );