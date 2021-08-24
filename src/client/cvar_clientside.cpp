#include "cvar_clientside.hpp"

ConVar::ConVar *fov;

ConVar::ConVar *scr_width;
ConVar::ConVar *scr_height;

void SetupClientSideConvars()
{
	fov = conVarHandle.DeclareConvar("fov", "70", ConVar::CVAR_ARCHIVE);

	scr_width = conVarHandle.DeclareConvar("scr_width", "856", ConVar::CVAR_ARCHIVE);
	scr_height = conVarHandle.DeclareConvar("scr_height", "482", ConVar::CVAR_ARCHIVE);
}