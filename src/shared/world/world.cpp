#include "world.hpp"

#include "utility/vector.hpp"
#include "chunk.hpp"

CWorld::CWorld( )
{
}

CWorld::~CWorld()
{
	for ( auto &&c : m_chunks )
		delete c.second;
}

CChunk *CWorld::getChunk( int x, int y, int z ) { return getChunk( Vector3f( x, y, z ) ); }

CChunk *CWorld::getChunk( const Vector3i &pos )
{
	if (m_chunks.find(pos) == m_chunks.end())
		return nullptr;
	return m_chunks.at(pos);
}

CChunk *CWorld::createChunk( int x, int y, int z ) { return createChunk( Vector3f( x, y, z ) ); }

CChunk *CWorld::createChunk( const Vector3i &pos )
{
	if ( m_chunks.find( pos ) != m_chunks.end() )
		return nullptr; // Couldn't create it

	CChunk *c = new CChunk( pos.x, pos.y, pos.z, m_chunkSize.x, m_chunkSize.y, m_chunkSize.z );

	m_chunks[pos] = c;
	c->m_world	  = this;

	return c;
}

CChunk *CWorld::getOrCreateChunk( int x, int y, int z ) { return getOrCreateChunk( Vector3f( x, y, z ) ); }

CChunk *CWorld::getOrCreateChunk( const Vector3i &pos )
{
	CChunk *c = getChunk( pos );
	return c != nullptr ? c : createChunk( pos );
}

CChunk *CWorld::getChunkWorldPos( int x, int y, int z ) { return getChunkWorldPos( Vector3f( x, y, z ) ); }

CChunk *CWorld::getChunkWorldPos( const Vector3i &pos )
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;

	worldPosToChunkPos( x, y, z );

	return getChunk( x, y, z );
}

Vector3f CWorld::chunkPosToWorldPos( const Vector3f &pos ) { return pos * m_chunkSize; }

Vector3f CWorld::worldPosToChunkPos( const Vector3f &pos ) { return pos / m_chunkSize; }

void CWorld::chunkPosToWorldPos( int &x, int &y, int &z )
{
	Vector3f pos = chunkPosToWorldPos( Vector3f( x, y, z ) );
	x			 = pos.x;
	y			 = pos.y;
	z			 = pos.z;
}

void CWorld::worldPosToChunkPos( int &x, int &y, int &z )
{
	Vector3f pos = worldPosToChunkPos( Vector3f( x, y, z ) );
	x			 = pos.x;
	y			 = pos.y;
	z			 = pos.z;
}

uint16_t CWorld::getID( int x, int y, int z )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return 0;

	return c->getID( x, y, z );
}

void CWorld::setID( int x, int y, int z, uint16_t id )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return;

	c->setID( x, y, z, id );
}

uint16_t CWorld::getMeta( int x, int y, int z )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return 0;

	return c->getMeta( x, y, z );
}

void CWorld::setMeta( int x, int y, int z, uint16_t meta )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return;

	c->setMeta( x, y, z, meta );
}

void CWorld::get( int x, int y, int z, uint16_t &id, uint16_t &meta )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
	{
		id	 = 0;
		meta = 0;
		return;
	}

	c->get( x, y, z, id, meta );
}

void CWorld::set( int x, int y, int z, uint16_t id, uint16_t meta )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return;

	c->set( x, y, z, id, meta );
}

void CWorld::setChunkSize( int x, int y, int z )
{
	m_chunkSize.x = x;
	m_chunkSize.y = y;
	m_chunkSize.z = z;
}

void CWorld::getChunkSize( int &x, int &y, int &z )
{
	x = m_chunkSize.x;
	y = m_chunkSize.y;
	z = m_chunkSize.z;
}

uint32_t CWorld::get( int x, int y, int z ) {
    // get in chunk coordinates
    int cx = x;
    int cy = y;
    int cz = z;
    worldPosToChunkPos(cx, cy, cz);

    x -= cx * m_chunkSize.x;
    y -= cy * m_chunkSize.y;
    z -= cz * m_chunkSize.z;

    CChunk *c = getChunk(cx, cy, cz);
    if (c == nullptr)
        return 0;

    return c->get(x, y, z);
}

void CWorld::set( int x, int y, int z, uint32_t data ) {
    // get in chunk coordinates
    int cx = x;
    int cy = y;
    int cz = z;
    worldPosToChunkPos(cx, cy, cz);

    x -= cx * m_chunkSize.x;
    y -= cy * m_chunkSize.y;
    z -= cz * m_chunkSize.z;

    CChunk *c = getChunk(cx, cy, cz);
    if (c == nullptr)
        return;

    c->set(x, y, z, data);
}

Vector3i CWorld::getChunkSize() { return m_chunkSize; }

std::vector<CChunk *> CWorld::getChunks() { 
	std::vector<CChunk *> list;
	for (auto &&c : m_chunks)
		list.push_back(c.second);

	return list;
}

void CWorld::clearChunks()
{
	for ( auto &&c : m_chunks )
		delete c.second;
	m_chunks.clear();
}

std::string CWorld::getName() { return m_name; }

void CWorld::setName( std::string name ) { m_name = name; }

uint32_t CWorld::getSeed() { return m_seed; }

void CWorld::setSeed( uint32_t seed ) { m_seed = seed; }

uint32_t CWorld::getTime() { return m_time; }

void CWorld::setTime( uint32_t time ) { m_time = time; }