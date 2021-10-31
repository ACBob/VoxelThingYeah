#include "chunk.hpp"

#include "world/world.hpp"

CChunk::CChunk(CVector pos, CWorld *pWorld)
{
	for (int i = 0; i < CHUNKLENGTH; i++)
	{
		m_blockID[i] = BLCK_AIR; // BLCK_NONE?
		m_value[i] = 0;
	}

	m_vPosition = pos;
	m_pWorld = pWorld;
}

CChunk::~CChunk()
{}

std::tuple<BLOCKID, BLOCKVAL> CChunk::GetBlockAtLocal(CVector pos)
{
	if (!ValidChunkPosition(pos))
		return {BLCK_NONE, 0};
	
	int i = CHUNK3D_TO_1D(pos.x, pos.y, pos.z);
	return {
		m_blockID[i],
		m_value[i]
	};
}

void CChunk::SetBlockAtLocal(CVector pos, BLOCKID blockId, BLOCKVAL val)
{
	if (!ValidChunkPosition(pos))
		return; // Display error?
	
	int i = CHUNK3D_TO_1D(pos.x, pos.y, pos.z);
	m_blockID[i] = blockId;
	m_value[i] = val;
}

std::tuple<BLOCKID, BLOCKVAL> CChunk::GetBlockAtIDX( int i )
{
	return {
		m_blockID[i],
		m_value[i]
	};
}

CVector CChunk::GetPosInWorld( CVector pos ) {
	CVector p = { m_vPosition.x / CHUNKSIZE_X, m_vPosition.y / CHUNKSIZE_Y, m_vPosition.z / CHUNKSIZE_Z };
	return p + pos;
}

CChunk *CChunk::Neighbour(Direction dir)
{
	return m_pWorld->ChunkAtPosNoCreate(m_vPosition + DirectionVector[dir]);
}

bool ValidChunkPosition(CVector pos)
{
	return (pos.x >= 0 && pos.x < CHUNKSIZE_X) &&
		(pos.y >= 0 && pos.y < CHUNKSIZE_Y) &&
		(pos.z >= 0 && pos.z < CHUNKSIZE_Z);
}