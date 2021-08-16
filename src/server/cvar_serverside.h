// Server-side ConVars.

#pragma once

#include "shared/cvar.h"

ConVar::ConVar *sv_timescale;
ConVar::ConVar *sv_tickms;

void SetupServerSideConvars()
{
	// TODO: How quickly time progresses
	sv_timescale = conVarHandle.DeclareConvar("sv_timescale", "1.0");
	
	// How many milliseconds there are in a tick
	// By default, 50 (or 20th of a second)
	sv_tickms = conVarHandle.DeclareConvar("sv_tickms", "50", ConVar::F::CVAR_CHEAT | ConVar::F::CVAR_SESSION);
}