#include "utility/vector.hpp"

#include "world/block.hpp"

#ifdef CLIENTEXE
	#include "rendering/chunkmodel.hpp"
	#include "rendering/shadermanager.hpp"
#endif

#include "fastnoise.h"

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
	// CVector can't be used in this context
	int32_t x, y, z;
	uint8_t m_iVal[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z], m_iValB[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];
	uint32_t m_iBlocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];
};

class CChunk
{
  public:
	CChunk();
	~CChunk();

	CChunk *Neighbour( Direction dir );

	CVector m_vPosition;
	CVector GetPosInWorld() { return m_vPosition * CVector( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ); }

	CBlock *GetBlockAtLocal( CVector pos );

#ifdef CLIENTEXE
	void RebuildMdl();

	void Render();
	void RenderTrans(); // Renders things that should be done last, like water
#endif
	void Update( int64_t iTick );

	CVector PosToWorld( CVector pos );

	// Flat array of blocks, access with
	// Indexed with [x + SIZEX * (y + SIZEZ * z)]
	CBlock m_blocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];

#ifdef CLIENTEXE
	CModel m_blocksMdl;
	CModel m_waterMdl;
	// 16 bits
	// 4 red, 4 green, 4 blue and 4 extra for the sky/natural light
	uint16_t m_iLightingValue[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];

	// Alpha (w) is the sun value
	Colour GetLightingLocal( CVector pos );
	void SetLightingLocal( CVector pos, Colour color );
#endif

	// World pointer (can't set type because circular include :lenny:)
	void *m_pChunkMan = nullptr;

	PortableChunkRepresentation m_portableDef;

	// Regular Dirt
	bool m_bDirty = false;

	int64_t m_iLastTick = 0;
};

bool ValidChunkPosition( CVector pos );