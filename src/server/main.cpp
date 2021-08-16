#include "enet/enet.h"

#include <cstdio>

#include "shared/filesystem.h"
#include "shared/network.h"

#include <chrono>

#include "utility/assorted.h"

#include "cvar_serverside.h"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include "world/world.h"

int main (int argc, char* args[]) {
	info("Hello from scenic bobcraft server!");
	info("Setting up Server-side convars...");
	SetupServerSideConvars();

	char *argstring = FlattenCharArray(args, 1, argc-1);
	debug("Args: %s", argstring);
	info("Parsing command line convars...");
	conVarHandle.Parse(argstring);

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

	info("Begin server chunkManager");
	World testWorld;

	info("Begin server main loop...");
	int64_t then = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count();
	int64_t now = then;
	int i = 0;
	while (true)
	{
		// Networking
		server.Update();


		now = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count();
		int64_t delta = now - then;

		if (now >= then)
		{
			then = now + sv_tickms->GetInt();
			i++;

			// TODO: NOT SEND EVERY TICK
			// server.sendWorld(world);
		}

	}

	return EXIT_SUCCESS;
}