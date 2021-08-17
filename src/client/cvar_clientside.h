// Client-side ConVars.

#pragma once

#include "shared/cvar.h"

ConVar::ConVar *convar_test;

ConVar::ConVar *scr_width;
ConVar::ConVar *scr_height;

void SetupClientSideConvars()
{
	convar_test = conVarHandle.DeclareConvar("convar_test", "1");

	scr_width = conVarHandle.DeclareConvar("scr_width", "856", ConVar::CVAR_ARCHIVE);
	scr_height = conVarHandle.DeclareConvar("scr_width", "482", ConVar::CVAR_ARCHIVE);
}