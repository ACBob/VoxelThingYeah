#include "server.hpp"

#include "logging.hpp"

#include "sound/soundmanager.hpp"

CNetworkServer::CNetworkServer( int port, int maxClients )
{
	m_addr.host = ENET_HOST_ANY;
	m_addr.port = port;

	m_pEnetHost = enet_host_create( &m_addr, maxClients, 1, 0, 0 );

	con_info( "Creating with port %i", port );

	if ( m_pEnetHost == NULL )
	{
		con_error( "Couldn't create ENet server object" );
		return;
	}

	if ( enet_host_compress_with_range_coder(m_pEnetHost) != 0 )
	{
		con_error("Couldn't enable compression!");
		return;
	}
}
CNetworkServer::~CNetworkServer()
{
	for ( CNetworkPlayer *c : m_players )
		KickPlayer( c, "Server is closing!" );
	enet_host_destroy( m_pEnetHost );
}

CNetworkPlayer *CNetworkServer::ClientFromUsername( const char *name )
{
	for ( CNetworkPlayer *c : m_players )
	{
		if ( c->m_username == name )
			return c;
	}

	return nullptr;
}
CNetworkPlayer *CNetworkServer::ClientFromPeer( ENetPeer *pPeer )
{
	for ( CNetworkPlayer *c : m_players )
	{
		if ( c->m_pPeer == pPeer )
			return c;
	}

	return nullptr;
}

void CNetworkServer::KickPlayer( ENetPeer *pPeer, const char *reason )
{
	CNetworkPlayer *c = ClientFromPeer( pPeer );
	if ( c == nullptr )
	{
		con_critical( "Tried to kick a pPeer without a client! What!" );
		return;
	}

	KickPlayer( c, reason );
}
void CNetworkServer::KickPlayer( const char *username, const char *reason )
{
	CNetworkPlayer *c = ClientFromUsername( username );
	if ( c == nullptr )
	{
		con_warning( "Tried to kick invalid username %s", username );
		return;
	}

	KickPlayer( c, reason );
}
CNetworkPlayer::~CNetworkPlayer()
{
	if ( m_pEntity != nullptr )
		m_pEntity->Kill();
}

void CNetworkServer::KickPlayer( CNetworkPlayer *player, const char *reason )
{
	ENetPeer *pPeer = player->m_pPeer;
	protocol::SendServerPlayerDisconnect( pPeer, true, reason );

	con_info( "Kicking player for reason %s", reason );

	enet_peer_disconnect_later( pPeer, NULL );
}

void CNetworkServer::Update()
{
	// Spend some time checking if anybody joins/leaves, or we get some client data
	ENetEvent e;
	while ( enet_host_service( m_pEnetHost, &e, 0 ) > 0 )
	{
		switch ( e.type )
		{
			break;
			case ENET_EVENT_TYPE_DISCONNECT: {
				CNetworkPlayer *c = ClientFromPeer( e.peer );
				con_info( "Goodbye %s!", c->m_username.c_str() );
				// Destroy the client object AND player
				m_players.erase( std::remove( m_players.begin(), m_players.end(), c ), m_players.end() );

				for ( CNetworkPlayer *cl : m_players )
				{
					protocol::SendServerPlayerLeave( cl->m_pPeer, c->m_username );
				}

				delete c;
			}
			break;
			case ENET_EVENT_TYPE_RECEIVE: {
				protocol::UncompressAndDealWithPacket(
					ArchiveIntermediary( e.packet->data, e.packet->data + e.packet->dataLength ), this, e.peer );
			}
			break;
		}
	}

	// Check for outdated chunks and queue them
	for ( auto &&c : m_world.m_chunks )
	{
		for ( CNetworkPlayer *cl : m_players )
		{
			if ( c->m_bReallyDirty )
				cl->m_pChunkQueue.push_back( c.get()->m_vPosition );
		}
	}

	for ( CNetworkPlayer *c : m_players )
	{
		if ( c->m_pEntity == nullptr )
			continue; // we can assume they probably no longer exist, or are in the process of joining/leaving or
					  // something

		// Update chunk pos
		// Unfortunate name
		CVector cP = ( c->m_pEntity->m_vPosition / CVector( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ).Floor();
		if ( cP != c->m_vChunkPos )
		{
			c->m_iLoadedChunkIDX	= 0;
			c->m_iNextChunkLoadTick = 0;
			c->m_pChunkQueue.push_back( cP );
		}
		c->m_vChunkPos = cP;

		if ( c->m_iLoadedChunkIDX >= ( 4 * 4 * 4 ) )
			continue;

		int x = 0;
		int y = 0;
		int z = 0;
		i1Dto3D( c->m_iLoadedChunkIDX, 4, 4, x, y, z );
		CVector p( x - 2, y - 2, z - 2 );
		c->m_iLoadedChunkIDX++;

		p = c->m_vChunkPos + p;

		// con_debug("QUEUE <%.0f,%.0f,%.0f>, %d", p.x, p.y, p.z, c->m_iLoadedChunkIDX);

		// Queue it
		c->m_pChunkQueue.push_back( p );
	}

	// Now handle the queue
	for ( CNetworkPlayer *p : m_players )
	{
		// They don't have an opportunity to load.
		if ( p->m_iNextChunkLoadTick > m_iCurrentTick )
			continue;
		p->m_iNextChunkLoadTick = m_iCurrentTick;
		// They have nothing in their queue
		if ( p->m_pChunkQueue.empty() )
			continue;

		CVector pos = p->m_pChunkQueue.back();
		p->m_pChunkQueue.pop_back();

		// con_debug("LOAD <%.0f,%.0f,%.0f>, %d", pos.x, pos.y, pos.z, p->m_iLoadedChunkIDX);

		CChunk *c = m_world.GetChunkGenerateAtWorldPos( pos * CVector( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) );

		protocol::SendServerChunkDataFromRep( p->m_pPeer, c->m_portableDef );
	}
}

bool CNetworkServer::WorkingServer() { return m_pEnetHost != NULL; }

void CNetworkServer::PlaySoundEvent(const char* name, CVector pos) {
	for (CNetworkPlayer* p : m_players) {
		if (p->m_pEntity == nullptr)
			continue;
		CVector dist = p->m_pEntity->m_vPosition - pos;
		if (dist.Magnitude() < SOUND_MAX_DISTANCE) {
			protocol::SendServerSoundEvent(p->m_pPeer, pos, name);
		}
	}
}