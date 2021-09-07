#include "server.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

CNetworkServer::CNetworkServer( int port, int maxClients )
{
	addr.host = ENET_HOST_ANY;
	addr.port = port;

	enetHost = enet_host_create( &addr, maxClients, 1, 0, 0 );

	con_info( "Creating with port %i", port );

	if ( enetHost == NULL )
	{
		con_error( "Couldn't create ENet server object" );
		return;
	}
}
CNetworkServer::~CNetworkServer()
{
	for ( CNetworkPlayer *c : players )
		KickPlayer( c, "Server is closing!" );
	enet_host_destroy( enetHost );
}

CNetworkPlayer *CNetworkServer::ClientFromUsername( const char *name )
{
	for ( CNetworkPlayer *c : players )
	{
		if ( c->username == name )
			return c;
	}

	return nullptr;
}
CNetworkPlayer *CNetworkServer::ClientFromPeer( ENetPeer *peer )
{
	for ( CNetworkPlayer *c : players )
	{
		if ( c->peer == peer )
			return c;
	}

	return nullptr;
}

void CNetworkServer::KickPlayer( ENetPeer *peer, const char *reason )
{
	CNetworkPlayer *c = ClientFromPeer( peer );
	if ( c == nullptr )
	{
		con_critical( "Tried to kick a peer without a client! What!" );
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
	ENetPeer *peer = player->peer;
	protocol::SendServerPlayerDisconnect( peer, true, reason );

	con_info( "Kicking player for reason %s", reason );

	enet_peer_disconnect_later( peer, NULL );
}

void CNetworkServer::Update()
{
	// Spend some time checking if anybody joins/leaves, or we get some client data
	ENetEvent e;
	while ( enet_host_service( enetHost, &e, 0 ) > 0 )
	{
		switch ( e.type )
		{
			break;
			case ENET_EVENT_TYPE_DISCONNECT: {
				CNetworkPlayer *c = ClientFromPeer( e.peer );
				con_info( "Goodbye %s!", c->username.c_str() );
				// Destroy the client object AND player
				players.erase( std::remove( players.begin(), players.end(), c ), players.end() );
				if ( c->entity != nullptr ) //! This is GOING to shoot me in the foot later
				{
					c->entity->Kill();
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
	for ( CChunk *c : world.chunks )
	{
		if ( c->outdated )
		{
			CWorld::PortableChunkRepresentation crep = world.GetWorldRepresentation( c->position );

			for ( CNetworkPlayer *cl : players )
			{
				if ( cl->nextChunkLoadTick < currentTick )
				{
					protocol::SendServerChunkDataFromRep( cl->peer, crep );
				}
			}
			c->outdated = false;
		}
	}

	for ( CNetworkPlayer *c : players )
	{
		// Update chunk pos
		// Unfortunate name
		CVector cP = ( c->entity->position / CVector( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ).Floor();
		if ( cP != c->chunkPos )
		{
			c->loadedChunkIDX	 = 0;
			c->nextChunkLoadTick = currentTick;
		}
		c->chunkPos = cP;

		if ( c->nextChunkLoadTick < currentTick )
			c->nextChunkLoadTick = currentTick + 1;
		else
			continue;

		if ( c->loadedChunkIDX >= ( 4 * 4 * 4 ) )
			continue;

		int x = 0;
		int y = 0;
		int z = 0;
		i1Dto3D( c->loadedChunkIDX, 4, 4, x, y, z );
		CVector p( x - 2, y - 2, z - 2 );

		p = p + c->chunkPos;

		protocol::SendServerChunkData( c->peer, &world, p );

		c->loadedChunkIDX++;
	}
}

bool CNetworkServer::WorkingServer() { return enetHost != NULL; }