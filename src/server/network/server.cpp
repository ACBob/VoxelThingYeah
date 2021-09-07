#include "server.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

NetworkServer::NetworkServer(int port, int maxClients)
{
	addr.host = ENET_HOST_ANY;
	addr.port = port;

	enetHost = enet_host_create(&addr, maxClients, 1, 0, 0);

	con_info("Creating with port %i", port);

	if (enetHost == NULL)
	{
		con_error("Couldn't create ENet server object");
		return;
	}
}
NetworkServer::~NetworkServer()
{
	for (NetworkPlayer *c : players)
		KickPlayer(c, "Server is closing!");
	enet_host_destroy(enetHost);
}

NetworkPlayer *NetworkServer::ClientFromUsername(const char *name)
{
	for (NetworkPlayer *c : players)
	{
		if (c->username == name)
			return c;
	}

	return nullptr;
}
NetworkPlayer *NetworkServer::ClientFromPeer(ENetPeer *peer)
{
	for (NetworkPlayer *c : players)
	{
		if (c->peer == peer)
			return c;
	}

	return nullptr;
}

void NetworkServer::KickPlayer(ENetPeer *peer, const char* reason)
{
	NetworkPlayer *c = ClientFromPeer(peer);
	if (c == nullptr)
	{
		con_critical("Tried to kick a peer without a client! What!");
		return;
	}

	KickPlayer(c, reason);
}
void NetworkServer::KickPlayer(const char *username, const char* reason)
{
	NetworkPlayer *c = ClientFromUsername(username);
	if (c == nullptr)
	{
		con_warning("Tried to kick invalid username %s", username);
		return;
	}

	KickPlayer(c, reason);
}
void NetworkServer::KickPlayer(Client *player, const char* reason)
{
	ENetPeer *peer = player->peer;
	protocol::messages::SendServerPlayerDisconnect(peer, true, reason);

	con_info("Kicking player for reason %s", reason);

	enet_peer_disconnect_later(peer, NULL);
}

void NetworkServer::Update()
{
	// Spend some time checking if anybody joins/leaves, or we get some client data
	ENetEvent e;
	while(enet_host_service(enetHost, &e, 0) > 0)
	{
		switch(e.type)
		{
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				Client* c = ClientFromPeer(e.peer);
				con_info("Goodbye %s!", c->username.c_str());
				// Destroy the client object AND player
				players.erase(
					std::remove(players.begin(), players.end(), c), players.end()
				);
				if (c->entity != nullptr) //! This is GOING to shoot me in the foot later
				{
					c->entity->Kill();
				}

				delete c;
			}
			break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				protocol::UncompressAndDealWithPacket(
					ArchiveIntermediary(e.packet->data, e.packet->data + e.packet->dataLength),
					this,
					e.peer);
			}
			break;
		}
	}

	// Check for outdated chunks and resend them to joined clients
	for (Chunk *c : world.chunks)
	{
		if (c->outdated)
		{
			World::PortableChunkRepresentation crep = world.GetWorldRepresentation(c->position);

			for (Client *cl : players)
			{
				if (cl->nextChunkLoadTick < currentTick)
				{
					protocol::messages::SendServerChunkDataFromRep(cl->peer, crep);
				}
			}
			c->outdated = false;
		}
	}

	for (Client *c : players)
	{
		// Update chunk pos
		// Unfortunate name
		Vector cP = (c->entity->position / Vector(CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z)).Floor();
		if (cP != c->chunkPos)
		{
			c->loadedChunkIDX = 0;
			c->nextChunkLoadTick = currentTick;
		}
		c->chunkPos = cP;


		if (c->nextChunkLoadTick < currentTick)
			c->nextChunkLoadTick = currentTick + 1;
		else
			continue;

		if (c->loadedChunkIDX >= (4*4*4))
			continue;
		
		int x = 0;
		int y = 0;
		int z = 0;
		i1Dto3D(c->loadedChunkIDX, 4, 4, x,y,z);
		Vector p(x - 2,y - 2,z - 2);

		p = p + c->chunkPos;

		protocol::messages::SendServerChunkData(c->peer, &world, p);
		
		c->loadedChunkIDX ++;
	}
	
}

bool NetworkServer::WorkingServer()
{
	return enetHost != NULL;
}