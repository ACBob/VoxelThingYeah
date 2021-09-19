#include "enet/enet.h"

#include <cstdio>

#include "network/network.hpp"
#include "network/server.hpp"
#include "shared/filesystem.hpp"

#include <chrono>

#include "utility/assorted.hpp"

#include "cvar_serverside.hpp"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include "world/world.hpp"

int main( int argc, char *args[] )
{
	con_info( "Hello from scenic bobcraft server!" );
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

	con_info( "Init Network..." );
	if ( !network::Init() )
	{
		con_critical( "Couldn't initialise Network! Unrecoverable!" );
		return EXIT_FAILURE;
	}
	atexit( network::Uninit );

	con_info( "Create Server..." );
	CNetworkServer server( sv_port->GetInt() );
	if ( !server.WorkingServer() )
	{
		con_critical( "Server became invalid" );
		return EXIT_FAILURE;
	}

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
			// Networking
			server.Update();
			server.m_iCurrentTick = i;

			// World
			server.m_world.WorldTick( i, sv_tickms->GetFloat() / 1000.0f );

			if ( i % 5 == 0 )
			{
				for ( CNetworkPlayer *c : server.m_players )
					protocol::SendServerTimeOfDay( c->m_pPeer, server.m_world.m_iTimeOfDay );
			}
		}
	}

	return EXIT_SUCCESS;
}