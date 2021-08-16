// Client-side ConVars.

#pragma once

#include "shared/cvar.h"

ConVar::ConVar *convar_test;

void SetupClientSideConvars()
{
	convar_test = conVarHandle.DeclareConvar("convar_test", "1");
}