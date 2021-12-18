// -- World.hpp
// Holds chunks in a QMap<Vector3f, CChunk*>
// and also some other useful things (like rendering a world)
// At some point, will also hold entities!

#include "utility/vector.hpp"

#include <vector>
#include <map>
#include <string>

#pragma once

class CChunk; // Forward Decl.

#include <inttypes.h>

class CWorld
{
  public:
	CWorld();
	~CWorld();

	CChunk *getChunk( int x, int y, int z );
	CChunk *getChunk( const Vector3i &coord );

	CChunk *createChunk( int x, int y, int z );
	CChunk *createChunk( const Vector3i &coord );

	CChunk *getOrCreateChunk( int x, int y, int z );
	CChunk *getOrCreateChunk( const Vector3i &coord );

	CChunk *getChunkWorldPos( int x, int y, int z );
	CChunk *getChunkWorldPos( const Vector3i &coord );

	// Converts from Chunk grid coords to world pos
	Vector3f chunkPosToWorldPos( const Vector3f &pos );

	// Converts from world pos to Chunk grid coords
	Vector3f worldPosToChunkPos( const Vector3f &pos );

	// Converts from Chunk grid coords to world pos
	void chunkPosToWorldPos( int &x, int &y, int &z );

	// Converts from world pos to Chunk grid coords
	void worldPosToChunkPos( int &x, int &y, int &z );

	uint16_t getID( int x, int y, int z );
	void setID( int x, int y, int z, uint16_t id );

	uint16_t getMeta( int x, int y, int z );
	void setMeta( int x, int y, int z, uint16_t meta );

	void get( int x, int y, int z, uint16_t &id, uint16_t &meta );
	void set( int x, int y, int z, uint16_t id, uint16_t meta );
	uint32_t get( int x, int y, int z );
	void set( int x, int y, int z, uint32_t data );

	void setChunkSize(int x, int y, int z);
	void getChunkSize(int &x, int &y, int &z);

	Vector3i getChunkSize();

	std::vector<CChunk *> getChunks();
	void clearChunks();

	std::string getName();
	void setName( std::string name );

	uint32_t getSeed();
	void setSeed( uint32_t seed );

	uint32_t getTime();
	void setTime( uint32_t time );

  private:
	std::map<Vector3i, CChunk *> m_chunks;

	// Luckily, chunk sizes are per world, not per chunk.
	// The first map format had it be per chunk, and that's silly.
	// Even luckier is that I rectified it *before* working on this and was able to avoid the headache that is.
	Vector3i m_chunkSize;

	std::string m_name;
	uint32_t m_seed;
	uint32_t m_time;
};