// Server-side ConVars.

#pragma once

#include "shared/cvar.h"

inline ConVar::ConVar *sv_timescale;
inline ConVar::ConVar *sv_tickms;

void SetupServerSideConvars();