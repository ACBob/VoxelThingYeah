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
#endif
	void Update();

	CVector PosToWorld( CVector pos );

	// Flat array of blocks, access with
	// Indexed with [x + SIZEX * (y + SIZEZ * z)]
	CBlock m_blocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];

#ifdef CLIENTEXE
	CModel m_mdl;
#endif

	// World pointer (can't set type because circular include :lenny:)
	void *m_pChunkMan = nullptr;

	bool m_bOutdated = false;
};

bool ValidChunkPosition( CVector pos );