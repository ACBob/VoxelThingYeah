#include "cvar_clientside.hpp"

ConVar::CConVar *fov = nullptr;

ConVar::CConVar *scr_width	= nullptr;
ConVar::CConVar *scr_height = nullptr;

ConVar::CConVar *username = nullptr;

ConVar::CConVar *cl_ip	 = nullptr;
ConVar::CConVar *cl_port = nullptr;

ConVar::CConVar *cl_servername = nullptr;

ConVar::CConVar *cl_volume = nullptr;
ConVar::CConVar *cl_reverb = nullptr;

ConVar::CConVar *playerskin = nullptr;

ConVar::CConVar *cl_kickreason = nullptr;

ConVar::CConVar *cl_resourcepacks = nullptr;

void SetupClientSideConvars()
{
	fov = conVarHandle.DeclareConvar( "fov", "70", ConVar::CVAR_ARCHIVE );

	scr_width  = conVarHandle.DeclareConvar( "scr_width", "848", ConVar::CVAR_ARCHIVE );
	scr_height = conVarHandle.DeclareConvar( "scr_height", "480", ConVar::CVAR_ARCHIVE );

	username = conVarHandle.DeclareConvar( "username", "Player", ConVar::CVAR_ARCHIVE );

	cl_ip	= conVarHandle.DeclareConvar( "cl_ip", "127.0.0.1" );
	cl_port = conVarHandle.DeclareConvar( "cl_port", "58008" );

	cl_servername = conVarHandle.DeclareConvar( "cl_servername", "", ConVar::CVAR_READONLY );

	cl_volume = conVarHandle.DeclareConvar( "cl_volume", "1.0", ConVar::CVAR_ARCHIVE );
	cl_reverb = conVarHandle.DeclareConvar( "cl_reverb", "true", ConVar::CVAR_ARCHIVE );

	playerskin = conVarHandle.DeclareConvar( "playerskin", "", ConVar::CVAR_ARCHIVE );

	cl_kickreason = conVarHandle.DeclareConvar( "cl_kickreason", "", ConVar::CVAR_READONLY );

	cl_resourcepacks = conVarHandle.DeclareConvar( "cl_resourcepacks", "", ConVar::CVAR_ARCHIVE );
}