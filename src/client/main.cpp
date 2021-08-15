#include "enet/enet.h"

#include "shared/filesystem.h"
#include "shared/network.h"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include <stdlib.h>
#include <cstdio>

int main (int argc, char* args[]) {

	info("Hello from bobcraft!");

	info("Init Filesystem...");
	if (!fileSystem::Init(args[0]))
	{
		critical("Couldn't initialise Filesystem! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(fileSystem::UnInit);

	info("Init Network...");
	if (!network::Init())
	{
		critical("Couldn't initialise Network! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(network::Uninit);

	info("Create Client...");
	network::Client client;
	if (!client.WorkingClient())
	{
		critical("Client ended up in invalid state!");
		return EXIT_FAILURE;
	}

	if (!client.Connect())
	{
		error("Didn't connect to anybody so we've nothing to do!");
		return EXIT_FAILURE;
	}

	client.Update();
	client.Disconnect();

	return EXIT_SUCCESS;
}