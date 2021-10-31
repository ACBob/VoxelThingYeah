#include "chunk.hpp"

CChunk::CChunk(CVector pos)
{
	for (int i = 0; i < CHUNKLENGTH; i++)
	{
		m_blockID[i] = BLCK_AIR; // BLCK_NONE?
		m_value[i] = 0;
	}

	m_vPosition = pos;
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