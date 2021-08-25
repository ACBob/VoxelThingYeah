#include "network.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

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
			peer = e.peer;
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
		con_info("Disconnecting");
		enet_peer_disconnect(peer, 0);

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
			enet_peer_disconnect_now(c.first, NULL); // TODO go away message
		enet_host_destroy(enetHost);
	}

	void Server::Update()
	{
		// Spend some time checking if anybody joins/leaves, or we get some client data
		ENetEvent e;
		while(enet_host_service(enetHost, &e, 0) > 0)
		{
			switch(e.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					con_info("Hello %x:%u!\n",
						e.peer->address.host,
						e.peer->address.port);
					// Create a client object
					EntityPlayer* p = new EntityPlayer();
					world.ents.push_back(p);
					Client* c = new Client{e.peer, p};
					players[e.peer] = c;
					con_info("Sending them 0,0");
					// SendWorld(e.peer, Vector(0));
				}
				break;
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					con_info("Goodbye %x:%u!\n",
						e.peer->address.host,
						e.peer->address.port);
					// Destroy the client object AND player
					Client* c = players[e.peer];
					c->entity->Kill();
					players.erase(players.find(e.peer));
				}
				break;
				case ENET_EVENT_TYPE_RECEIVE:
				{
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
