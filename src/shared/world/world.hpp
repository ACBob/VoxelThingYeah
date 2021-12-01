#include "world/block.hpp"
#include "world/chunk.hpp"

#include "utility/vector.hpp"

#include "physics.hpp"
#include "world/block.hpp"

#include "jeneration/jeneration.hpp"

#pragma once

#include <map>
#include <vector>

// Forward Decl.
class CEntityBase;

#include <memory>

class CWorld
{
  public:
	CWorld();
	~CWorld();

	CChunk *ChunkAtWorldPos( Vector3f pos );
	CChunk *ChunkAtChunkPos( Vector3f chunkPos );

	// Tries to get a chunk and generates a new one if it can't find one
	CChunk *GetChunkGenerateAtWorldPos( Vector3f pos );
	CChunk *GetChunkGenerateAtPos( Vector3f pos );

	// Deletes the chunk at the chunk position
	void UnloadChunk( Vector3f pos );

	// Returns the block at the position in world coords
	// The given position is rounded by floor() before being used
	// If outside the world it returns a nullptr
	block_t *BlockAtWorldPos( Vector3f pos );

	// The correct way to set a block
	// Also takes into account that the block may be in a chunk that is not loaded
	void SetBlockAtWorldPos( Vector3f pos, BLOCKID block, BLOCKVAL val = 0 );

	// Test against an infinitely small point centred on pos
	// Tests in world coordinates
	bool TestPointCollision( Vector3f pos );

	// Test against an infinitely small point centred on pos
	// Tests in chunk coordinates
	// This test ignores whether or not you can collide with the block, it just checks if you can select it
	bool TestSelectPointCollision( Vector3f pos );

	// Tests in world coordinates
	// Returning the first block that collides
	block_t *TestAABBCollision( CBoundingBox col );

	// Is the position within our place
	bool ValidChunkPos( const Vector3f pos );

	// Tick is the tick since the start of the game
	// TODO: This will overflow maybe
	void WorldTick( int64_t tick, float delta );

	int64_t m_iLastTick = 0;

	// Merges the PortableChunkRepresentation into us :)
	void UsePortable( PortableChunkRepresentation rep );
	// Returns the world rep at the chunk
	PortableChunkRepresentation GetWorldRepresentation( Vector3f pos );

	std::vector<CEntityBase *> m_ents;
	void AddEntity( CEntityBase *e );

	CEntityBase *GetEntityByName( const char *name );

	std::vector<std::unique_ptr<CChunk>> m_chunks;

	// Keep track of blocks placed in chunks not yet loaded
	// TODO: better data structure?
	std::map<Vector3f, uint16_t> m_blocksToPlace;

	int m_iTimeOfDay = 6890;

	COverworldJeneration m_jenerator;
};