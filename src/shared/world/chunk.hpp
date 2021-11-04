#pragma once

#include "utility/vector.hpp"

#ifdef CLIENTEXE
	#include "rendering/chunkmodel.hpp"
	#include "rendering/shadermanager.hpp"
#endif

#include "blockdef.hpp"

#include "direction.hpp"

#include <tuple>

// Forward decl.
class CWorld;

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
	CChunk(CVector pos, CWorld *pWorld);
	~CChunk();

	// Returns {BLOCKID, META}
	// Coords local to chunk
	std::tuple<BLOCKID, BLOCKVAL> GetBlockAtLocal( CVector pos );

	// Sets the block at the local coordinates
	void SetBlockAtLocal( CVector pos, BLOCKID block, BLOCKVAL val = 0 );

	// Gets the block at the idx
	// Functionally equivalent to m_blockID[i]
	std::tuple<BLOCKID, BLOCKVAL> GetBlockAtIDX( int i );

	// Sets the block at the idx
	// Functionally equivalent to m_blockID[i] =
	void SetBlockAtIDX( int i, BLOCKID id, BLOCKVAL val = 0 );

	// Returns the position corrected to world coordinates relative to this chunk
	CVector GetPosInWorld( CVector pos = {0,0,0} );

	// Returns the chunk that is next to us in direction (if it exists)
	CChunk *Neighbour(Direction dir);

	// Tests if the *local* position is blocking and therefore do no light or faces
	bool IsObscuringBlockAt( CVector pos );

	void Tick( int64_t tick );

#ifdef CLIENTEXE
	// Rebuilds the model
	void RebuildModel();
	
	// Renders just the blocks, without liquid
	void Render();

	// Renders all the liquid
	void RenderLiquid();

	CModel m_blockModel;
	CModel m_liquidModel;
#endif

	// Should we update
	bool m_bDirty = false;

	// Used for networking or saving
	ChunkData m_data;

  protected:
	BLOCKID m_blockID[CHUNKLENGTH];
	BLOCKVAL m_value[CHUNKLENGTH];

	CVector m_vPosition;

	CWorld *m_pWorld;
};

bool ValidChunkPosition( CVector pos );