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
class CBoundingBox; // Forward Decl.

#include "entities/entities.hpp"

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

	CEntity *getEntity( int id );
	CEntity *getEntity( const std::string &name );
	std::vector<CEntity *> getEntitiesByName( const std::string &name );

	template <typename T>
	T *createEntity()
	{
		// TODO: better id management
		T *e = new T();
		e->setworld(this);
		m_entities.push_back( e );
		return e;
	}

	// Converts from Chunk grid coords to world pos
	virtual Vector3f chunkPosToWorldPos( const Vector3f &pos );

	// Converts from world pos to Chunk grid coords
	virtual Vector3f worldPosToChunkPos( const Vector3f &pos );

	// Converts from Chunk grid coords to world pos
	virtual void chunkPosToWorldPos( int &x, int &y, int &z );

	// Converts from world pos to Chunk grid coords
	virtual void worldPosToChunkPos( int &x, int &y, int &z );

	virtual uint16_t getID( int x, int y, int z );
	virtual void setID( int x, int y, int z, uint16_t id );

	virtual uint16_t getMeta( int x, int y, int z );
	virtual void setMeta( int x, int y, int z, uint16_t meta );

	virtual void get( int x, int y, int z, uint16_t &id, uint16_t &meta );
	virtual void set( int x, int y, int z, uint16_t id, uint16_t meta );
	virtual uint32_t get( int x, int y, int z );
	virtual void set( int x, int y, int z, uint32_t data );

	virtual void setChunkSize(int x, int y, int z);
	virtual void getChunkSize(int &x, int &y, int &z);

	virtual Vector3i getChunkSize();

	virtual std::vector<CChunk *> getChunks();
	virtual void clearChunks();

	virtual std::string getName();
	virtual void setName( std::string name );

	virtual uint32_t getSeed();
	virtual void setSeed( uint32_t seed );

	virtual uint32_t getTime();
	virtual void setTime( uint32_t time );

	virtual bool testCollision( CBoundingBox &box );

	virtual void update(float dt);

  protected:
	std::map<Vector3i, CChunk *> m_chunks;
	std::vector<CEntity *> m_entities;

	// Luckily, chunk sizes are per world, not per chunk.
	// The first map format had it be per chunk, and that's silly.
	// Even luckier is that I rectified it *before* working on this and was able to avoid the headache that is.
	Vector3i m_chunkSize;

	std::string m_name;
	uint32_t m_seed;
	uint32_t m_time;
};