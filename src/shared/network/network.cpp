#include "network.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#ifdef CLIENTEXE
#include "cvar_clientside.hpp"
#elif SERVEREXE
#include "cvar_serverside.hpp"
#endif

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
			{
				ClientPacket p;
				p.type = ClientPacket::PLAYER_ID;
				Archive<ArchiveBuf> bufAccess = p.GetAccess();
				bufAccess << PROTOCOL_VERSION;
				bufAccess << std::string(username->GetString());

				protocol::SendPacket(e.peer, p);
			}
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
		{
			ClientPacket p;
			Archive<ArchiveBuf> bufAccess = p.GetAccess();
			p.type = ClientPacket::LEAVE;

			protocol::SendPacket(peer, p);
			enet_peer_disconnect(peer, 0);
		}

		con_info("Taking some time to ignore packets sent our way...");
		while (enet_host_service(enetHost, &e, 3000) > 0)
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
		for (auto c : players)
			KickPlayer(c.second, "Server is closing!");
		enet_host_destroy(enetHost);
	}

	void Server::KickPlayer(ENetPeer *peer, const char* reason)
	{
		KickPlayer(players[peer], reason);
	}
	void Server::KickPlayer(Client *player, const char* reason)
	{
		ENetPeer *peer = player->peer;

		ServerPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type = ServerPacket::PLAYER_DISCONNECT;
		
		bufAccess << true;
		bufAccess << std::string(reason);

		con_info("Kicking player for reason %s", reason);

		protocol::SendPacket(peer, p);
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
					Client* c = players[e.peer];
					con_info("Goodbye %s!", c->username.c_str());
					// Destroy the client object AND player
					players.erase(players.find(e.peer));
					if (c->entity != nullptr) //! This is GOING to shoot me in the foot later
					{
						c->entity->Kill();
					}
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
