#include "network.h"

#include "seethe.h"

namespace network
{
	bool Init()
	{
		if(enet_initialize() != 0)
		{
			error("ENet failed to initialise");
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
			error("Couldn't create ENet client object");
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
			error("No peers available.");
			return false;
		}

		info("We've found a peer!");
		info("Giving server 2.5 seconds to respond...");
		if (enet_host_service(enetHost, &e, 2500) > 0 && e.type == ENET_EVENT_TYPE_CONNECT)
		{
			info("Hello! We've connected to a server!");
		}
		else
		{
			error("Nobody said hello back :(");
			return false;
		}
	}
	void Client::Disconnect()
	{
		info("Disconnecting");
		enet_peer_disconnect(peer, 0);

		info("Taking some time to ignore packets sent our way...");
		while (enet_host_service(enetHost, &e, 3000) > 0)
		{
			switch (e.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy(e.packet);
				break;
				case ENET_EVENT_TYPE_DISCONNECT:
					info("Disconnected. Bye bye server! :)");
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
			error("Couldn't create ENet server object");
			return;
		}
	}
	Server::~Server()
	{
		enet_host_destroy(enetHost);
	}
	
	bool Server::WorkingServer()
	{
		return enetHost != NULL;
	}
#endif
}