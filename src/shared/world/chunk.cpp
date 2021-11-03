#include "chunk.hpp"

#include "world/world.hpp"

CChunk::CChunk(CVector pos, CWorld *pWorld)
{
	for (int i = 0; i < CHUNKLENGTH; i++)
	{
		m_blockID[i] = BLCK_AIR; // BLCK_NONE?
		m_value[i] = 0;

		m_data.m_iBlocks[i] = BLCK_AIR;
		m_value[i] = 0;
		
	}

	m_data.x = pos.x;
	m_data.y = pos.y;
	m_data.z = pos.z;

	m_vPosition = pos;
	m_pWorld = pWorld;

#ifdef CLIENTEXE
	m_blockModel.m_vPosition = GetPosInWorld();
	m_liquidModel.m_vPosition = m_blockModel.m_vPosition;
#endif
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
void CChunk::SetBlockAtIDX( int i, BLOCKID id, BLOCKVAL val )
{
	m_blockID[i] = id;
	m_value[i] = val;
}

CVector CChunk::GetPosInWorld( CVector pos ) {
	CVector p = { m_vPosition.x * CHUNKSIZE_X, m_vPosition.y * CHUNKSIZE_Y, m_vPosition.z * CHUNKSIZE_Z };
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

void CChunk::Tick( int64_t tick )
{
	if (m_bDirty)
	{
		// Rebuild the portable information at last
		m_data.x = m_vPosition.x;
		m_data.y = m_vPosition.y;
		m_data.z = m_vPosition.z;

		for ( int j = 0; j < CHUNKLENGTH; j++ )
		{
			m_data.m_iBlocks[j] = m_blockID[j];
			m_data.m_iValue[j]  = m_value[j];
		}

#ifdef CLIENTEXE
		RebuildModel();
		for ( int i = 0; i < 6; i++ )
		{
			CChunk *neighbour = Neighbour( (Direction)i );
			if ( neighbour != nullptr )
				neighbour->RebuildModel();
		}
#endif

		m_bDirty = false;
	}
}

#ifdef CLIENTEXE
void CChunk::RebuildModel() {
	BuildChunkModel( m_blockModel, m_liquidModel, m_blockID, m_value, this );
}

void CChunk::Render() {
	m_blockModel.Render();
}
void CChunk::RenderLiquid() {
	m_liquidModel.Render();
}
#endif