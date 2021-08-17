#include "enet/enet.h"

#include "shared/filesystem.h"
#include "shared/network.h"
#include "shared/cvar.h"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include <stdlib.h>
#include <cstdio>

#include "utility/assorted.h"
#include "cvar_clientside.h"

int main (int argc, char* args[]) {

	con_info("Hello from bobcraft!");
	con_info("Setting up client-side convars...");
	SetupClientSideConvars();

	char *argstring = FlattenCharArray(args, 1, argc-1);
	con_debug("Args: %s", argstring);
	con_info("Parsing command line convars...");
	conVarHandle.Parse(argstring);


	con_debug("Value of CVar 'convar_test': %s", conVarHandle.FindConVar("convar_test")->GetString());

	con_info("Init Filesystem...");
	if (!fileSystem::Init(args[0]))
	{
		con_critical("Couldn't initialise Filesystem! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(fileSystem::UnInit);

	con_info("Init Network...");
	if (!network::Init())
	{
		con_critical("Couldn't initialise Network! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(network::Uninit);

	con_info("Create Client...");
	network::Client client;
	if (!client.WorkingClient())
	{
		con_critical("Client ended up in invalid state!");
		return EXIT_FAILURE;
	}

	if (!client.Connect())
	{
		con_error("Didn't connect to anybody so we've nothing to do!");
		return EXIT_FAILURE;
	}

	while (true)
	{
		client.Update();
	}


	client.Disconnect();

	return EXIT_SUCCESS;
}