#include "enet/enet.h"

#include <cstdio>

#include "network/network.hpp"
#include "network/server.hpp"
#include "shared/filesystem.hpp"

#include <chrono>

#include "utility/assorted.hpp"

#include "cvar_serverside.hpp"

#include "shared/logging.hpp"

#include "world/world.hpp"

#include "sound/soundmanager.hpp"

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

	con_info( "Parsing svconfig.cfg..." );
	bool succeed;
	int64_t l;
	char *file = (char *)fileSystem::LoadFile( "usr/svconfig.cfg", l, succeed );
	if ( succeed )
		conVarHandle.Parse( file );

	con_info( "Init Network..." );
	if ( !network::Init() )
	{
		con_critical( "Couldn't initialise Network! Unrecoverable!" );
		return EXIT_FAILURE;
	}
	atexit( network::Uninit );

	con_info( "Init Sound System..." );
	soundSystem::Init();
	atexit( soundSystem::UnInit );

	con_info( "Create Server..." );
	CNetworkServer server( sv_port->GetInt() );
	if ( !server.WorkingServer() )
	{
		con_critical( "Server became invalid" );
		return EXIT_FAILURE;
	}

	soundSystem::server = &server;

	con_info( "Begin server main loop..." );
	int64_t then =
		std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() )
			.count();
	int64_t now	   = then;
	unsigned int i = 0;
	while ( true )
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
			server.m_world.WorldTick( i, sv_tickms->GetFloat() / 1000.0f );

			// Networking
			server.Update();
			server.m_iCurrentTick = i;

			if ( i % 40 == 0 )
			{
				for ( CNetworkPlayer *c : server.m_players )
					protocol::SendServerTimeOfDay( c->m_pPeer, server.m_world.m_iTimeOfDay );
			}
		}
	}

	conVarHandle.WriteCFG("svconfig.cfg");

	return EXIT_SUCCESS;
}