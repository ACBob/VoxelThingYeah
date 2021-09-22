#include "server.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

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
				if ( c->m_pEntity != nullptr ) //! This is GOING to shoot me in the foot later
				{
					c->m_pEntity->Kill();
				}

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

	// Check for outdated chunks and resend them to joined clients
	// TODO: Queue
	for ( CChunk *c : m_world.m_chunks )
	{
		if ( c->m_bReallyDirty )
		{
			for ( CNetworkPlayer *cl : m_players )
			{
				if ( cl->m_iNextChunkLoadTick < m_iCurrentTick )
				{
					protocol::SendServerChunkDataFromRep( cl->m_pPeer, c->m_portableDef );
				}
			}

			c->m_bReallyDirty = false;
		}
	}

	for ( CNetworkPlayer *c : m_players )
	{
		// Update chunk pos
		// Unfortunate name
		CVector cP = ( c->m_pEntity->m_vPosition / CVector( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ).Floor();
		if ( cP != c->m_vChunkPos )
		{
			c->m_iLoadedChunkIDX	= 0;
			c->m_iNextChunkLoadTick = m_iCurrentTick;
		}
		c->m_vChunkPos = cP;

		if ( c->m_iNextChunkLoadTick < m_iCurrentTick )
			c->m_iNextChunkLoadTick = m_iCurrentTick + 1;
		else
			continue;

		if ( c->m_iLoadedChunkIDX >= ( 4 * 4 * 4 ) )
			continue;

		int x = 0;
		int y = 0;
		int z = 0;
		i1Dto3D( c->m_iLoadedChunkIDX, 4, 4, x, y, z );
		CVector p( x - 2, y - 2, z - 2 );

		p = p + c->m_vChunkPos;

		protocol::SendServerChunkData( c->m_pPeer, &m_world, p );

		c->m_iLoadedChunkIDX++;
	}
}

bool CNetworkServer::WorkingServer() { return m_pEnetHost != NULL; }