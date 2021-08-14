#include "enet/enet.h"

#include <cstdio>

int main (int argc, char* args[]) {
	if (enet_initialize() != 0)
	{
		printf("Couldn't init Enet!\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

	ENetAddress addr;
	ENetEvent e;
	addr.host = ENET_HOST_ANY;
	addr.port = 58008; // Haha boobs

	ENetHost *server;
	server = enet_host_create(&addr, 8, 1, 0, 0);

	if (server == NULL)
	{
		printf("Couldn't create Enet Host\n");
		return EXIT_FAILURE;
	}

	while (true)
	{

		while(enet_host_service(server, &e, 1000) > 0)
		{
			switch(e.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					printf("New Client Connection: %x:%u\n",
						e.peer->address.host,
						e.peer->address.port);
				break;
				case ENET_EVENT_TYPE_DISCONNECT:
					printf("Goodbye %x:%u!\n",
						e.peer->address.host,
						e.peer->address.port);
				break;
			}
		}

	}

	enet_host_destroy(server);

	return EXIT_SUCCESS;
}