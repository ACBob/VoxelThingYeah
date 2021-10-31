#include "utility/vector.hpp"

#ifdef CLIENTEXE
	#include "rendering/chunkmodel.hpp"
	#include "rendering/shadermanager.hpp"
#endif

#include "blockdef.hpp"

#include <tuple>

#define CHUNKSIZE_X 16
#define CHUNKSIZE_Y 16
#define CHUNKSIZE_Z 16
#define CHUNKLENGTH CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z

// Why yes, I am a C++ Programmer, how could you tell?
#define CHUNK3D_TO_1D( x, y, z ) x + y *CHUNKSIZE_X + z *CHUNKSIZE_X *CHUNKSIZE_Z
#define CHUNK1D_TO_3D( i, x, y, z )                                                                                    \
	z = round( i / ( CHUNKSIZE_X * CHUNKSIZE_Y ) );                                                                    \
	y = round( ( i - z * CHUNKSIZE_X * CHUNKSIZE_Y ) / CHUNKSIZE_X );                                                  \
	x = i - CHUNKSIZE_X * ( y + CHUNKSIZE_Y * z )

#pragma once

// A storage only type of chunk that stores JUST essential information to rebuild a chunk
struct ChunkData
{
	// CVector can't be used in this context
	int32_t x, y, z;
	uint64_t m_iBlocks[CHUNKLENGTH];
	uint16_t m_iValue[CHUNKLENGTH];
};

class CChunk
{
  public:
	CChunk(CVector pos);
	~CChunk();

	// Returns {BLOCKID, META}
	// Coords local to chunk
	std::tuple<BLOCKID, BLOCKVAL> GetBlockAtLocal( CVector pos );

	// Sets the block at the local coordinates
	void SetBlockAtLocal( CVector pos, BLOCKID block, BLOCKVAL val );

	// Used for networking or saving
	ChunkData m_data;

  protected:
	BLOCKID m_blockID[CHUNKLENGTH];
	BLOCKVAL m_value[CHUNKLENGTH];

	CVector m_vPosition;

  private:
#ifdef CLIENTEXE
	CModel m_blockModel;
	CModel m_liquidModel;
#endif
};

bool ValidChunkPosition( CVector pos );