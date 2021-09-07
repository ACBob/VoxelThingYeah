#include "client.hpp"


NetworkClient::NetworkClient()
{
	enetHost = enet_host_create(NULL, 1, 1, 0, 0);

	if (enetHost == NULL)
	{
		con_error("Couldn't create ENet client object");
		return;
	}
}
NetworkClient::~NetworkClient()
{
	enet_host_destroy(enetHost);
}

bool NetworkClient::WorkingClient()
{
	return enetHost != NULL;
}

bool NetworkClient::Connect(const char* address, int port)
{
	if(enet_address_set_host_ip(&addr, address) != 0)
	{
		con_error("Could not set the IP");
		return false;
	}
	addr.port = port;

	con_info("Connecting to %s:%i", address, port);

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
void NetworkClient::Disconnect()
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

void NetworkClient::Update()
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

	Vector cP = (localPlayer->position / Vector(CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z)).Floor();
	for (Chunk *c : localWorld->chunks)
	{
		if ((c->position - cP).Magnitude() > 4)
		{
			localWorld->UnloadChunk(c->position);
		}
	}
}
