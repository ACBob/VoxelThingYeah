#include "network.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#ifdef CLIENTEXE
#include "cvar_clientside.hpp"
#elif SERVEREXE
#include "cvar_serverside.hpp"
#endif

#include <algorithm>

#include <sstream>

namespace network
{
	bool Init()
	{
		if(enet_initialize() != 0)
		{
			con_error("ENet failed to initialise");
			return false;
		}
		return true;
	}
	void Uninit()
	{
		enet_deinitialize();
	}

#ifdef CLIENTEXE
	Client::Client()
	{
		enetHost = enet_host_create(NULL, 1, 1, 0, 0);

		if (enetHost == NULL)
		{
			con_error("Couldn't create ENet client object");
			return;
		}
	}
	Client::~Client()
	{
		enet_host_destroy(enetHost);
	}

	bool Client::WorkingClient()
	{
		return enetHost != NULL;
	}

	bool Client::Connect(const char* address, int port)
	{
		enet_address_set_host(&addr, address);
		addr.port = port;

		peer = enet_host_connect(enetHost, &addr, 1, 0);
		if (!peer)
		{
			con_error("No peers available.");
			return false;
		}

		con_info("We've found a peer!");
		con_info("Giving server 2.5 seconds to respond...");
		if (enet_host_service(enetHost, &e, 2500) > 0 && e.type == ENET_EVENT_TYPE_CONNECT)
		{
			con_info("Hello! We've connected to a server!");
			con_info("Sending the neccesary information");
			protocol::messages::SendClientPlayerID(e.peer);
			peer = e.peer;
			connected = true;
			return true;
		}
		else
		{
			con_error("Nobody said hello back :(");
			return false;
		}
	}
	void Client::Disconnect()
	{
		if (!connected || peer == nullptr)
		{
			con_warning("Disconnect without connection");
			return;
		}
		con_info("Disconnecting");
		protocol::messages::SendClientLeave(peer);

		while (enet_host_service(enetHost, &e, 0) > 0)
		{
			switch (e.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy(e.packet);
				break;
				case ENET_EVENT_TYPE_DISCONNECT:
					con_info("Disconnected. Bye bye server! :)");
				break;
			}
		}

		peer = nullptr;
		connected = false;
	}

	void Client::Update()
	{
		ENetEvent e;
		if (!peer) 
		{
			con_warning("Attempt to update server without a peer!");
			return; // Can't update w/out a peer
		}

		while(enet_host_service(enetHost, &e, 0) > 0)
		{
			switch (e.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
				{
					protocol::UncompressAndDealWithPacket(
						ArchiveIntermediary(e.packet->data, e.packet->data + e.packet->dataLength),
						this);
				}
				break;
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					con_info("Oh, the server has said goodbye... :(");
					Disconnect();
					return;
				}
				break;
			}
		}
	}

#elif SERVEREXE
	Server::Server(int port, int maxClients)
	{
		addr.host = ENET_HOST_ANY;
		addr.port = port;

		enetHost = enet_host_create(&addr, maxClients, 1, 0, 0);

		if (enetHost == NULL)
		{
			con_error("Couldn't create ENet server object");
			return;
		}
	}
	Server::~Server()
	{
		for (Client *c : players)
			KickPlayer(c, "Server is closing!");
		enet_host_destroy(enetHost);
	}

	Client *Server::ClientFromUsername(const char *name)
	{
		for (Client *c : players)
		{
			if (c->username == name)
				return c;
		}

		return nullptr;
	}
	Client *Server::ClientFromPeer(ENetPeer *peer)
	{
		for (Client *c : players)
		{
			if (c->peer == peer)
				return c;
		}

		return nullptr;
	}

	void Server::KickPlayer(ENetPeer *peer, const char* reason)
	{
		Client *c = ClientFromPeer(peer);
		if (c == nullptr)
		{
			con_critical("Tried to kick a peer without a client! What!");
			return;
		}

		KickPlayer(c, reason);
	}
	void Server::KickPlayer(const char *username, const char* reason)
	{
		Client *c = ClientFromUsername(username);
		if (c == nullptr)
		{
			con_warning("Tried to kick invalid username %s", username);
			return;
		}

		KickPlayer(c, reason);
	}
	void Server::KickPlayer(Client *player, const char* reason)
	{
		ENetPeer *peer = player->peer;
		protocol::messages::SendServerPlayerDisconnect(peer, true, reason);

		con_info("Kicking player for reason %s", reason);

		enet_peer_disconnect_later(peer, NULL);
	}

	void Server::Update()
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
	
		// send entity information to peers
		
	}
	
	bool Server::WorkingServer()
	{
		return enetHost != NULL;
	}
#endif
}