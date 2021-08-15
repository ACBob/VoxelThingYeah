#include "enet/enet.h"

#include "shared/filesystem.h"
#include "shared/logging.h"

#include <stdlib.h>
#include <cstdio>

int main (int argc, char* args[]) {

	log::log(log::INFO, "Hello from Bobcraft!");
	log::log(log::WARN, "Testing Log module...");
	log::log(log::ERRR, "DON'T PANIC");
	log::log(log::DBUG, "%d, Dudes!", 69);

	if (fileSystem::Init(args[0]) != 0)
	{
		log::log(log::ERRR, "Couldn't initialise filesystem! Fatal!");
		return EXIT_FAILURE;
	}
	atexit(fileSystem::UnInit);

	if (enet_initialize() != 0)
	{
		log::log(log::ERRR, "Couldn't init Enet!\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);


	ENetHost *client;
	client = enet_host_create(NULL, 1, 1, 0, 0);

	if (client == NULL)
	{
		log::log(log::ERRR, "Couldn't create Enet Host\n");
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
		log::log(log::ERRR, "No Peers\n");
		return EXIT_FAILURE;
	}

	if (enet_host_service(client, &e, 5000) > 0 && e.type == ENET_EVENT_TYPE_CONNECT)
	{
		log::log(log::INFO, "Connected to server! Hello!\n");
	}
	else
	{
		enet_peer_reset(peer);
		log::log(log::ERRR, "Connection failed :(\n");
		return EXIT_SUCCESS;
	}

	while (enet_host_service(client, &e, 1000))
	{
		switch(e.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
				log::log(log::DBUG, "Packet (L: %u, C: %s) from %s.",
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
				log::log(log::INFO, "Disconnected. Bye bye! :)\n");
			break;
		}
	}

	return EXIT_SUCCESS;
}