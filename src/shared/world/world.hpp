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

#include <memory>

class CWorld
{
  public:
#ifdef CLIENTEXE
	CWorld( CShader *shader, CShader *entShader, CShader *waterShader, CTexture *worldTex );
#elif SERVEREXE
	CWorld();
#endif
	~CWorld();

	CChunk *ChunkAtWorldPos( Vector3f pos );
	CChunk *ChunkAtChunkPos( Vector3f chunkPos );

	// Tries to get a chunk and generates a new one if it can't find one
	CChunk *GetChunkGenerateAtWorldPos( Vector3f pos );

	// Deletes the chunk at the chunk position
	void UnloadChunk( Vector3f pos );

	// Returns the block at the position in world coords
	// The given position is rounded by floor() before being used
	// If outside the world it returns a nullptr
	CBlock *BlockAtWorldPos( Vector3f pos );

	// Test against an infinitely small point centred on pos
	// Tests in world coordinates
	bool TestPointCollision( Vector3f pos );

	// Tests in world coordinates
	// Returning the first block that collides
	CBlock* TestAABBCollision( CBoundingBox col );

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

	std::vector<void *> m_ents;
	void AddEntity( void *e );

	void *GetEntityByName( const char *name );

#ifdef CLIENTEXE
	void Render();

	CColour GetLightingAtWorldPos( Vector3f pos );

	void *m_pLocalPlayer = nullptr;

	// Shader we render with
	CShader *m_pWorldShader = nullptr;
	// Shader entities render with
	CShader *m_pEntityShader = nullptr;
	// Shader we render specifically water with
	CShader *m_pWaterShader = nullptr;

	// Texture we use to render the world
	CTexture *m_pWorldTex = nullptr;
#endif

	std::vector<std::unique_ptr<CChunk>> m_chunks;

	int m_iTimeOfDay = 6890;

	COverworldJeneration m_jenerator;
};