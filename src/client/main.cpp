#include "enet/enet.h"

#include "shared/filesystem.h"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include <stdlib.h>
#include <cstdio>

int main (int argc, char* args[]) {

	info("Hello from bobcraft!");

	if (fileSystem::Init(args[0]) != 0)
	{
		critical("Couldn't initialise Filesystem! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(fileSystem::UnInit);

	if (enet_initialize() != 0)
	{
		critical("Couldn't initialise Enet! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);


	ENetHost *client;
	client = enet_host_create(NULL, 1, 1, 0, 0);

	if (client == NULL)
	{
		critical("Couldn't create Enet host! Unrecoverable!");
		return EXIT_FAILURE;
	}

	ENetAddress addr;
	ENetEvent e;
	ENetPeer* peer;

	enet_address_set_host(&addr, "127.0.0.1");
	addr.port = 58008;

	peer = enet_host_connect(client, &addr, 1, 0);
	if (peer == NULL)
	{
		error("No peers.");
		return EXIT_FAILURE;
	}

	if (enet_host_service(client, &e, 5000) > 0 && e.type == ENET_EVENT_TYPE_CONNECT)
	{
		info("Hello! We've connected to a server!");
	}
	else
	{
		enet_peer_reset(peer);
		error("No server responed. :(");
		return EXIT_SUCCESS;
	}

	while (enet_host_service(client, &e, 1000))
	{
		switch(e.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
				debug("Packet (L: %u, C: %s) from %s.",
					e.packet->dataLength,
					e.packet->data,
					e.peer->data
				);
			break;
		}
	}

	enet_peer_disconnect(peer, 0);

	while (enet_host_service(client, &e, 3000) > 0)
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

	return EXIT_SUCCESS;
}