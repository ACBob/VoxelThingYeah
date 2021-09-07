#include "cvar_serverside.hpp"

ConVar::CConVar *sv_timescale = nullptr;
ConVar::CConVar *sv_tickms	 = nullptr;

ConVar::CConVar *sv_name = nullptr;
ConVar::CConVar *sv_desc = nullptr;

ConVar::CConVar *sv_port = nullptr;

void SetupServerSideConvars()
{
	// TODO: How quickly time progresses
	sv_timescale = conVarHandle.DeclareConvar( "sv_timescale", "1.0" );

	// How many milliseconds there are in a tick
	// By default, 50 (or 20th of a second)
	sv_tickms = conVarHandle.DeclareConvar( "sv_tickms", "50", ConVar::F::CVAR_CHEAT | ConVar::F::CVAR_SESSION );

	// Name & Description of the server
	sv_name = conVarHandle.DeclareConvar( "sv_name", "Epic Bobcraft Server", ConVar::F::CVAR_SESSION );
	sv_desc = conVarHandle.DeclareConvar( "sv_desc", "Cool Server", ConVar::F::CVAR_SESSION );

	// Port to open
	sv_port = conVarHandle.DeclareConvar( "sv_port", "58008", ConVar::F::CVAR_ARCHIVE );
}