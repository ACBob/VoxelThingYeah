#include "cvar_clientside.hpp"

ConVar::ConVar *fov = nullptr;

ConVar::ConVar *scr_width  = nullptr;
ConVar::ConVar *scr_height = nullptr;

ConVar::ConVar *username = nullptr;

ConVar::ConVar *cl_ip	= nullptr;
ConVar::ConVar *cl_port = nullptr;

ConVar::ConVar *cl_servername = nullptr;

void SetupClientSideConvars()
{
	fov = conVarHandle.DeclareConvar( "fov", "70", ConVar::CVAR_ARCHIVE );

	scr_width  = conVarHandle.DeclareConvar( "scr_width", "856", ConVar::CVAR_ARCHIVE );
	scr_height = conVarHandle.DeclareConvar( "scr_height", "482", ConVar::CVAR_ARCHIVE );

	username = conVarHandle.DeclareConvar( "username", "Player", ConVar::CVAR_ARCHIVE );

	cl_ip	= conVarHandle.DeclareConvar( "cl_ip", "127.0.0.1" );
	cl_port = conVarHandle.DeclareConvar( "cl_port", "58008" );

	cl_servername = conVarHandle.DeclareConvar( "cl_servername", "", ConVar::CVAR_READONLY );
}