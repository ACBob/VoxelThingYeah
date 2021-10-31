#pragma once

#include <map>
#include <memory>

#include "vector.hpp"
#include "chunk.hpp"

#include "entities/entitybase.hpp"


class CWorld
{
  public:
	CWorld();
	~CWorld();

	// Returns the chunk at the position in chunk coordinates
	// Returns nullptr if it does not presently exist
	CChunk *ChunkAtPosNoCreate(CVector pos);

	// Returns the chunk if it exists, else creates and returns
	CChunk *ChunkAtPosCreate(CVector pos);

	// Same as ChunkAtWorldPosNoCreate but in world coordinates
	CChunk *ChunkAtWorldPosNoCreate(CVector pos);

	// Same as ChunkAtPosCreate but in world coordinates
	CChunk *ChunkAtWorldPos(CVector pos);

	// Gets the block at the world pos
	// Block ID is BLCK_NONE if outside the world or invalid
	std::tuple<BLOCKID, BLOCKVAL> GetBlockAtWorldPos( CVector pos );

	void SetBlockAtWorldPos( CVector pos, BLOCKID id, BLOCKVAL val = 0 );

	// Returns the position and ID of the block we hit first
	// On error, the W coordinate of the pos is 1 & the block id is BLCK_NONE
	std::tuple<CVector, BLOCKID> TestAABBCollision( CBoundingBox col );

	// Adds an entity to the world, taking ownership of it
	CEntityBase *AddEntity(std::unique_ptr<CEntityBase> ent);

	void WorldTick( int nTick, float delta );

	// The time in the day
	int m_iWorldTime = 0;

  protected:
	std::map<CVector, std::unique_ptr<CChunk>> m_chunks;
	std::vector<std::unique_ptr<CEntityBase>> m_entities;
};