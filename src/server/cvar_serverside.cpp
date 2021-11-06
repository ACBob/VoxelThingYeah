#include "cvar_serverside.hpp"

#include "logging.hpp"

ConVar::CConVar *sv_timescale = nullptr;
ConVar::CConVar *sv_tickms	  = nullptr;

ConVar::CConVar *sv_name = nullptr;
ConVar::CConVar *sv_desc = nullptr;

ConVar::CConVar *sv_port = nullptr;

ConVar::CConVar *sv_run = nullptr;

void closeCmd(const char *args)
{
	con_info("Closing server...");
	sv_run->SetBool(false);
}

void addCmd(const char *args)
{
	// add two numbers (testing command)
	float a, b;
	sscanf(args, "%f %f", &a, &b);
	con_info("%f %c %f = %f", a, b < 0 ? '-' : '+', b * (b < 0 ? -1 : 1), a + b);
}

void SetupServerSideConvars()
{
	// TODO: How quickly time progresses
	sv_timescale = conVarHandle.DeclareConvar( "sv_timescale", "1.0" );

	// How many milliseconds there are in a tick
	// By default, 50 (or 20th of a second)
	sv_tickms = conVarHandle.DeclareConvar( "sv_tickms", "50", ConVar::F::CVAR_CHEAT | ConVar::F::CVAR_SESSION );

	// Name & Description of the server
	sv_name = conVarHandle.DeclareConvar( "sv_name", "Epic Meegreef Server", ConVar::F::CVAR_ARCHIVE );
	sv_desc = conVarHandle.DeclareConvar( "sv_desc", "Cool Server", ConVar::F::CVAR_ARCHIVE );

	// Port to open
	sv_port = conVarHandle.DeclareConvar( "sv_port", "58008", ConVar::F::CVAR_ARCHIVE );

	// If the server should run
	sv_run = conVarHandle.DeclareConvar( "sv_run", "true", ConVar::F::CVAR_SESSION );

	// Close the server
	conVarHandle.DeclareConCmd( "close", "", closeCmd);
	// Add two numbers
	conVarHandle.DeclareConCmd( "add", "ff", addCmd);
}