// Server-side ConVars.

#pragma once

#include "shared/cvar.hpp"

extern ConVar::ConVar *sv_timescale;
extern ConVar::ConVar *sv_tickms;

extern ConVar::ConVar *sv_name;
extern ConVar::ConVar *sv_desc;

extern ConVar::ConVar *sv_port;

void SetupServerSideConvars();