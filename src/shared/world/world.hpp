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

	CChunk *ChunkAtWorldPos( Vector3i pos );
	CChunk *ChunkAtChunkPos( Vector3i chunkPos );

	// Tries to get a chunk and generates a new one if it can't find one
	CChunk *GetChunkGenerateAtWorldPos( Vector3i pos );
	CChunk *GetChunkGenerateAtPos( Vector3i pos );

	// Deletes the chunk at the chunk position
	void UnloadChunk( Vector3i pos );

	// Returns the block at the position in world coords
	// The given position is rounded by floor() before being used
	// If outside the world it returns a nullptr
	block_t *BlockAtWorldPos( Vector3i pos );

	// The correct way to set a block
	// Also takes into account that the block may be in a chunk that is not loaded
	void SetBlockAtWorldPos( Vector3i pos, BLOCKID block, BLOCKVAL val = 0 );

	// Test against an infinitely small point centred on pos
	// Tests in world coordinates
	bool TestPointCollision( Vector3i pos );

	// Test against an infinitely small point centred on pos
	// Tests in chunk coordinates
	// This test ignores whether or not you can collide with the block, it just checks if you can select it
	bool TestSelectPointCollision( Vector3i pos );

	// Tests in world coordinates
	// Returning the first block that collides
	block_t *TestAABBCollision( CBoundingBox col );

	// Is the position within our place
	bool ValidChunkPos( const Vector3i pos );

	// Tick is the tick since the start of the game
	// TODO: This will overflow maybe
	void WorldTick( int64_t tick, float delta );

	int64_t m_iLastTick = 0;

	// Merges the PortableChunkRepresentation into us :)
	void UsePortable( PortableChunkRepresentation rep );
	// Returns the world rep at the chunk
	PortableChunkRepresentation GetWorldRepresentation( Vector3i pos );

	std::vector<CEntityBase *> m_ents;
	void AddEntity( CEntityBase *e );

	CEntityBase *GetEntityByName( const char *name );

	std::map<Vector3i, std::unique_ptr<CChunk>> m_chunks;

	int m_iTimeOfDay = 6890;

	COverworldJeneration m_jenerator;
};