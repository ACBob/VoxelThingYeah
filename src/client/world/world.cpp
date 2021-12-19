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
	for (auto e : m_entities) {
		e->Render();
	}
}

void CClientWorld::update(float dt) {
	for (auto e : m_entities) {
		e->Update(dt);
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

CClientEntityBase *CClientWorld::getEntity( int id )
{
	if ( id < 0 || id >= m_entities.size() )
		return nullptr;

	return m_entities[id];
}

CClientEntityBase *CClientWorld::getEntity( const std::string &name )
{
	for ( auto &&e : m_entities )
		if ( e->GetName() == name )
			return e;

	return nullptr;
}

std::vector<CClientEntityBase *> CClientWorld::getEntitiesByName( const std::string &name )
{
	std::vector<CClientEntityBase *> ret;

	for ( auto &&e : m_entities )
		if ( e->GetName() == name )
			ret.push_back( e );

	return ret;
}