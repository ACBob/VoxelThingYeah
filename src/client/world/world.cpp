#include "world.hpp"

#include "chunk.hpp"

CClientWorld::CClientWorld() {}
CClientWorld::~CClientWorld() {
    clearChunks();
}

void CClientWorld::render() {
    for (auto &&c : m_chunks) {
        c.second->render();
	}
	// now entities
	for (CEntity *e : m_entities) {
		if (!e->hasModel() || !e->getvisible()) continue;

		rendering::models::CModel *mdl = rendering::models::GetModel(e->getModelName());
		if (!mdl)
			continue;
		
		mdl->Render(
			e->getposition(),
			e->getrotation(),
			e->getscale()
		);
	}
}

void CClientWorld::update(float dt) {
	for (CEntity *e : m_entities) {
		e->update(dt);
	}
}


CClientChunk *CClientWorld::getChunk( int x, int y, int z ) { return getChunk( Vector3f( x, y, z ) ); }

CClientChunk *CClientWorld::getChunk( const Vector3i &pos )
{
	if (m_chunks.find(pos) == m_chunks.end())
		return nullptr;
	return m_chunks.at(pos);
}

CClientChunk *CClientWorld::createChunk( int x, int y, int z ) { return createChunk( Vector3f( x, y, z ) ); }

CClientChunk *CClientWorld::createChunk( const Vector3i &pos )
{
	if ( m_chunks.find( pos ) != m_chunks.end() )
		return nullptr; // Couldn't create it

	CClientChunk *c = new CClientChunk( pos.x, pos.y, pos.z, m_chunkSize.x, m_chunkSize.y, m_chunkSize.z );

	m_chunks[pos] = c;
	c->m_world	  = this;

	return c;
}

CClientChunk *CClientWorld::getOrCreateChunk( int x, int y, int z ) { return getOrCreateChunk( Vector3f( x, y, z ) ); }

CClientChunk *CClientWorld::getOrCreateChunk( const Vector3i &pos )
{
	CClientChunk *c = getChunk( pos );
	return c != nullptr ? c : createChunk( pos );
}

CClientChunk *CClientWorld::getChunkWorldPos( int x, int y, int z ) { return getChunkWorldPos( Vector3f( x, y, z ) ); }

CClientChunk *CClientWorld::getChunkWorldPos( const Vector3i &pos )
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;

	worldPosToChunkPos( x, y, z );

	return getChunk( x, y, z );
}

uint16_t CClientWorld::getID( int x, int y, int z )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CClientChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return 0;

	return c->getID( x, y, z );
}

void CClientWorld::setID( int x, int y, int z, uint16_t id )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CClientChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return;

	c->setID( x, y, z, id );
}

uint16_t CClientWorld::getMeta( int x, int y, int z )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CClientChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return 0;

	return c->getMeta( x, y, z );
}

void CClientWorld::setMeta( int x, int y, int z, uint16_t meta )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CClientChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return;

	c->setMeta( x, y, z, meta );
}

void CClientWorld::get( int x, int y, int z, uint16_t &id, uint16_t &meta )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CClientChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return;

	c->get( x, y, z, id, meta );
}

void CClientWorld::set( int x, int y, int z, uint16_t id, uint16_t meta )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CClientChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return;

	c->set( x, y, z, id, meta );
}

uint32_t CClientWorld::get( int x, int y, int z )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CClientChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return 0;

	return c->get( x, y, z );
}

void CClientWorld::set( int x, int y, int z, uint32_t data )
{
	// get in chunk coordinates
	int cx = x;
	int cy = y;
	int cz = z;
	worldPosToChunkPos( cx, cy, cz );

	x -= cx * m_chunkSize.x;
	y -= cy * m_chunkSize.y;
	z -= cz * m_chunkSize.z;

	CClientChunk *c = getChunk( cx, cy, cz );
	if ( c == nullptr )
		return;

	c->set( x, y, z, data );
}