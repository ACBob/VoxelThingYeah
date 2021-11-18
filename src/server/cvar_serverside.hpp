// Server-side ConVars.

#pragma once

#include "shared/cvar.hpp"

extern ConVar::CConVar *sv_timescale;
extern ConVar::CConVar *sv_tickms;

extern ConVar::CConVar *sv_name;
extern ConVar::CConVar *sv_desc;

extern ConVar::CConVar *sv_port;

extern ConVar::CConVar *sv_run;

void SetupServerSideConvars();