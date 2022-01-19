#include "enet/enet.h"

#include <cstdio>

#include "network/sv_network.hpp"
#include "shared/filesystem.hpp"

#include <chrono>

#include "utility/assorted.hpp"

#include "cvar_serverside.hpp"

#include "shared/logging.hpp"

#include "world/world.hpp"

#include <thread>

#include <iostream>

#ifdef _WIN32
	#include <windows.h>
#endif

int main( int argc, char *args[] )
{
	// Because Windows is from the stoneage
#ifdef _WIN32
	SetConsoleOutputCP( CP_UTF8 );
#endif
	con_info( "Hello from scenic Meegreef server!" );
	con_info( "Setting up Server-side convars..." );
	SetupServerSideConvars();

	char *argstring = FlattenCharArray( args, 1, argc - 1 );
	con_debug( "Args: %s", argstring );
	con_info( "Parsing command line convars..." );
	conVarHandle.Parse( argstring );

	con_info( "Init Filesystem..." );
	if ( !fileSystem::Init( args[0] ) )
	{
		con_critical( "Couldn't initialise Filesystem! Unrecoverable!" );
		return EXIT_FAILURE;
	}
	atexit( fileSystem::UnInit );

	// write in the current directory
	if ( !fileSystem::MountWrite( "." ) )
		con_warning( "Couldn't mount current directory for writing!" );
	if ( !fileSystem::Mount( ".", "/" ) )
		con_warning( "Couldn't mount current directory for reading!" );

	con_info( "Parsing svconfig.cfg..." );
	bool succeed;
	int64_t l;
	char *file = (char *)fileSystem::LoadFile( "svconfig.cfg", l, succeed );
	if ( succeed )
		conVarHandle.Parse( file );

	// con_info( "Init Network..." );
	// if ( !network::Init() )
	// {
	// 	con_critical( "Couldn't initialise Network! Unrecoverable!" );
	// 	return EXIT_FAILURE;
	// }
	// atexit( network::Uninit );

	con_info( "Create Server..." );
	CServer server( 27015, sv_maxclients->GetInt() );
	if ( !true ) // TODO: validate server
	{
		con_critical( "Server became invalid" );
		return EXIT_FAILURE;
	}

	con_info( "Create World..." );
	CWorld world;

	server.m_pWorld = &world;

	// Thread for getting input from the console
	// TODO: move to a more generic fashion so that the client can also have it, and try to achieve readline-like functionality
	// std::thread consoleThread( []() {
	// 	while ( sv_run->GetBool() )
	// 	{
	// 		// read from stdin
	// 		char *input = new char[1024];
	// 		std::cin.getline( input, 1024 );
	// 		if ( input )
	// 		{
	// 			con_info( "Console input: %s", input );
	// 			conVarHandle.Parse( input );
	// 		}
	// 		else
	// 			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	// 	}
	// } );

	con_info( "Begin server main loop..." );
	int64_t then =
		std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() )
			.count();
	int64_t now	   = then;
	unsigned int i = 0;
	while ( sv_run->GetBool() )
	{
		now =
			std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() )
				.count();
		int64_t delta = now - then;

		if ( now >= then ) // TICK
		{
			then = now + sv_tickms->GetInt();
			i++;

			// World
			// world.Tick( i, delta );
		}

		// Networking
		server.Update();
	}

	for ( CClient *c : server.m_clients )
	{
		server.KickPlayer( c, "server.shutdown" );
		server.Update();
	}

	// consoleThread.join();
	conVarHandle.WriteCFG( "svconfig.cfg" );

	return EXIT_SUCCESS;
}