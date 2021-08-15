#include "enet/enet.h"

#include <cstdio>

#include "shared/filesystem.h"
#include "shared/network.h"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

int main (int argc, char* args[]) {
	info("Hello from scenic bobcraft server!");

	if (!fileSystem::Init(args[0]))
	{
		critical("Couldn't initialise Filesystem! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(fileSystem::UnInit);

	if (!network::Init())
	{
		critical("Couldn't initialise Enet! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(network::Uninit);

	network::Server server;
	if (!server.WorkingServer())
	{
		critical("Server became invalid");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}