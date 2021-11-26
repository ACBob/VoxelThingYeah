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

	if ( enet_host_compress_with_range_coder( m_pEnetHost ) != 0 )
	{
		con_error( "Couldn't enable compression!" );
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
			// TODO: range check
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
		Vector3f cP = ( c->m_pEntity->m_vPosition / Vector3f( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ).Floor();
		if ( cP != c->m_vChunkPos )
		{
			c->m_iLoadedChunkIDX	= 0;
			c->m_iNextChunkLoadTick = 0;
			// The old chunk queue is probably invalid now, so clear it
			// But also queue the chunk they're immediately in
			// (Don't clear the load queue, it's still valid and cleared later)
			// TODO: Should we clear dirty chunks?
			c->m_pChunkQueue.clear();
			c->m_pChunkQueue.push_back( cP );

			c->m_iSpiralX = c->m_iSpiralZ = 0;
			c->m_iSpiralDX = 0;
			c->m_iSpiralDZ = -1;
		}
		c->m_vChunkPos = cP;

		// https://stackoverflow.com/questions/398299/looping-in-a-spiral
		// We can substitute the max(x,y)**2 with a known value as render distance is hard-coded
		// So that's 6 render distance, or 36 chunks

		if ( c->m_iLoadedChunkIDX >= 36 )
			continue; // Nothing to do! they've loaded all their chunks.

		if ( (-3 <= c->m_iSpiralX) && ( c->m_iSpiralX <= 3 ) && (-3 <= c->m_iSpiralZ) && ( c->m_iSpiralZ <= 3 ) )
		{
			// Vertical render distance is hard-coded to 3
			// Loading 1 below, 1 at the same level, and 1 above
			for (int y = 0; y < 3; y++)
			{
				// Ok now the spiralling is out the way, we can work out 'p'.
				Vector3f p = c->m_vChunkPos + Vector3f( c->m_iSpiralX, y - 1, c->m_iSpiralZ );		

				// Queue it, only if it's not already queued and the player hasn't already loaded it
				if ( std::find( c->m_pChunkQueue.begin(), c->m_pChunkQueue.end(), p ) == c->m_pChunkQueue.end() &&
					std::find( c->m_pChunkSent.begin(), c->m_pChunkSent.end(), p ) == c->m_pChunkSent.end() )
				{
					c->m_pChunkQueue.push_back( p );
				}
			}
		}

		if ( (c->m_iSpiralX == c->m_iSpiralZ) || (c->m_iSpiralX < 0 && c->m_iSpiralX == -c->m_iSpiralZ) || (c->m_iSpiralX > 0 && c->m_iSpiralX == 1 - c->m_iSpiralZ) )
		{
			std::swap( c->m_iSpiralDX, c->m_iSpiralDZ );
			c->m_iSpiralDX = -c->m_iSpiralDX;
		}

		c->m_iSpiralX += c->m_iSpiralDX;
		c->m_iSpiralZ += c->m_iSpiralDZ;
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

		Vector3f pos = p->m_pChunkQueue.back();
		p->m_pChunkQueue.pop_back();

		// con_debug( "SEND <%.0f,%.0f,%.0f>, %d", pos.x, pos.y, pos.z, p->m_iLoadedChunkIDX );

		CChunk *c = m_world.GetChunkGenerateAtPos( pos );

		protocol::SendServerChunkDataFromRep( p->m_pPeer, c->m_portableDef );

		p->m_pChunkSent.push_back( pos );
	}

	// Now for each player, test if they have any chunks outside of their view distance and remove it from sent
	for ( CNetworkPlayer *p : m_players )
	{
		if ( p->m_pEntity == nullptr )
			continue;

		Vector3f pos	  = p->m_pEntity->m_vPosition;
		Vector3f chunkPos = ( pos / Vector3f( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ).Floor();

		for ( Vector3f &v : p->m_pChunkSent )
		{
			if ( v.Distance( chunkPos ) > 6 )
			{
				p->m_pChunkSent.erase( std::remove( p->m_pChunkSent.begin(), p->m_pChunkSent.end(), v ),
									   p->m_pChunkSent.end() );

				// con_debug( "REMOVED <%.0f,%.0f,%.0f>", v.x, v.y, v.z );
			}
		}
	}
}

bool CNetworkServer::WorkingServer() { return m_pEnetHost != NULL; }

void CNetworkServer::PlaySoundEvent( const char *name, Vector3f pos )
{
	for ( CNetworkPlayer *p : m_players )
	{
		if ( p->m_pEntity == nullptr )
			continue;
		Vector3f dist = p->m_pEntity->m_vPosition - pos;
		if ( dist.Magnitude() < SOUND_MAX_DISTANCE )
		{
			protocol::SendServerSoundEvent( p->m_pPeer, pos, name );
		}
	}
}