#include "network.h"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <bitsery/brief_syntax.h>

#include <sstream>

namespace network
{
	// Forward decl
	struct NetworkPacket;

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
					con_info("We've recieved something, assuming it's chunk data :trollface:");
					NetworkPacket p = GetPacketBack(e.packet);
					
					switch (p.type)
					{
						case NetworkPacket::CHUNKDATA:
							DecodeChunkData(p.data);
						break;
					}
				break;
			}
		}
	}

	void Client::DecodeChunkData(std::vector<uint> data)
	{
		std::vector<unsigned char> buf(data.begin(), data.end());
		World::PortableChunkRepresentation crep;
		auto state = bitsery::quickDeserialization(
			bitsery::InputBufferAdapter<std::vector<unsigned char>>{buf.begin(), data.size()}, crep
		);

		// Woot, data!
		localWorld->UsePortable(crep);
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
		enet_host_destroy(enetHost);
	}

	void Server::Update(World *world)
	{
		ENetEvent e;
		while(enet_host_service(enetHost, &e, 0) > 0)
		{
			switch(e.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					con_info("Hello %x:%u!\n",
						e.peer->address.host,
						e.peer->address.port);
					con_info("Sending them 0,0");
					SendWorld(world, e.peer, Vector(0));
				break;
				case ENET_EVENT_TYPE_DISCONNECT:
					con_info("Goodbye %x:%u!\n",
						e.peer->address.host,
						e.peer->address.port);
				break;
			}
		}
	}
	
	bool Server::WorkingServer()
	{
		return enetHost != NULL;
	}

	void Server::SendWorld(World *world, ENetPeer *peer, Vector pos)
	{
		World::PortableChunkRepresentation chunkRep = world->GetWorldRepresentation(pos);
		
		BitseryBuf buf;

		size_t writtenSize = bitsery::quickSerialization<bitsery::OutputBufferAdapter<BitseryBuf>>(buf, chunkRep);

		SendPacket(peer, NetworkPacket::CHUNKDATA, buf);
	}
#endif

	void SendPacket(ENetPeer *peer, NetworkPacket::type_t type, BitseryBuf data)
	{		
		BitseryBuf buf;
		size_t writtenSize = bitsery::quickSerialization<bitsery::OutputBufferAdapter<BitseryBuf>>(buf, 
			NetworkPacket{
				(uint32_t)type,
				std::vector<uint32_t>(data.begin(), data.end())
			}
		);
		ENetPacket *packet = enet_packet_create(&buf[0], writtenSize, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
	}
	NetworkPacket GetPacketBack(ENetPacket *packet)
	{
		BitseryBuf buf(packet->data, packet->data + packet->dataLength);
		NetworkPacket p;

		auto state = bitsery::quickDeserialization<
			bitsery::InputBufferAdapter<BitseryBuf>
		>({buf.begin(), packet->dataLength}, p);

		return p;
	}
}
