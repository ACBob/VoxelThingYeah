#include "world/block.hpp"
#include "world/chunk.hpp"

#include "utility/vector.hpp"

#ifdef CLIENTEXE
	#include "rendering/shadermanager.hpp"
#endif

#include "physics.hpp"
#include "world/block.hpp"

#include "jeneration/jeneration.hpp"

#pragma once

#include <vector>

// #include "entities/entitybase.h"

class CWorld
{
  public:
#ifdef CLIENTEXE
	CWorld( CShader *shader, CShader *entShader );
#elif SERVEREXE
	CWorld();
#endif
	~CWorld();

	CChunk *ChunkAtWorldPos( CVector pos );
	CChunk *ChunkAtChunkPos( CVector chunkPos );

	// Tries to get a chunk and generates a new one if it can't find one
	CChunk *GetChunkGenerateAtWorldPos( CVector pos );

	// Deletes the chunk at the chunk position
	void UnloadChunk( CVector pos );

	// Returns the block at the position in world coords
	// The given position is rounded by floor() before being used
	// If outside the world it returns a nullptr
	CBlock *BlockAtWorldPos( CVector pos );

	// Test against an infinitely small point centred on pos
	// Tests in world coordinates
	bool TestPointCollision( CVector pos );

	// Tests in world coordinates
	bool TestAABBCollision( CBoundingBox col );

	// Is the position within our place
	bool ValidChunkPos( const CVector pos );

	// Tick is the tick since the start of the game
	// FIXME: depending on if I got my calculation right, this will shit itself either in 1,000 or so years or 3.
	void WorldTick( int tick );

	struct PortableChunkRepresentation
	{
		int32_t x, y, z;
		uint32_t blocks[CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z];
	};
	PortableChunkRepresentation GetWorldRepresentation( CVector pos );

	// Merges the PortableChunkRepresentation into us :)
	void UsePortable( PortableChunkRepresentation rep );

	std::vector<void *> m_ents;
	void AddEntity( void *e );

	void *GetEntityByName( const char *name );

#ifdef CLIENTEXE
	void Render();

	void *m_pLocalPlayer = nullptr;

	// Shader we render with
	CShader *m_pWorldShader = nullptr;
	// Shader entities render with
	CShader *m_pEntityShader = nullptr;
#endif

	std::vector<CChunk *> m_chunks;

	int m_iTimeOfDay = 6890;

	COverworldJeneration m_jenerator;
};