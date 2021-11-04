#pragma once

#include <map>
#include <memory>

#include "vector.hpp"
#include "chunk.hpp"

#include "jeneration/jeneration.hpp"

#include "entities/entitybase.hpp"

#ifdef CLIENTEXE
#include "rendering/shadermanager.hpp"
#include "rendering/texturemanager.hpp"
#endif

class CWorld
{
  public:
#ifdef CLIENTEXE
	CWorld(CShader *blockShader, CShader *entityShader, CShader *waterShader, CTexture *worldTex);
#else
	CWorld();
#endif
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

	// Returns the ID and Value of the block we hit
	// Position can therefore be inferred as the floor of p
	std::tuple<BLOCKID, BLOCKVAL> PointCollision( CVector p );
	// Returns if we actually collided with anything, rather than the outcome of it
	bool TestPointCollision( CVector p );

	// Returns the position and ID of the block we hit first
	// On error, the W coordinate of the pos is 1 & the block id is BLCK_NONE
	std::tuple<CVector, BLOCKID> AABBCollision( CBoundingBox col );
	// Returns if we actually collided with anything, rather than the outcome of it
	bool TestAABBCollision( CBoundingBox col );

	// Adds an entity to the world, taking ownership of it
	CEntityBase *AddEntity(std::unique_ptr<CEntityBase> ent);

	// Finds the first entity named such
	CEntityBase *GetEntityByName( std::string name );

	// Loads the data into a chunk
	void LoadFromData(ChunkData data);

	void WorldTick( int nTick, float delta );

	size_t GetChunkCount() {
		return m_chunks.size();
	}

#ifdef CLIENTEXE
	void Render();

	CTexture* m_pWorldTex;
	CShader* m_pBlockShader;
	CShader* m_pEntityShader;
	CShader* m_pLiquidShader;
#endif

	// The time in the day
	int m_iWorldTime = 6000;

	// Last time we ticked
	int64_t m_iLastTick = 0;

	COverworldJeneration m_worldJenerator;

  protected:
	std::map<CVector, std::unique_ptr<CChunk>> m_chunks;
	std::vector<std::unique_ptr<CEntityBase>> m_entities;
};