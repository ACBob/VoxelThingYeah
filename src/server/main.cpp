#include "enet/enet.h"

#include <cstdio>

#include "shared/filesystem.h"
#include "shared/network.h"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

int main (int argc, char* args[]) {
	info("Hello from scenic bobcraft server!");

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

	info("Create Server...");
	network::Server server;
	if (!server.WorkingServer())
	{
		critical("Server became invalid");
		return EXIT_FAILURE;
	}

	info("Begin server main loop...");
	while (true)
	{
		server.Update();
	}

	return EXIT_SUCCESS;
}