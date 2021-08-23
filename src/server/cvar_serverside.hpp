// Server-side ConVars.

#pragma once

#include "shared/cvar.hpp"

inline ConVar::ConVar *sv_timescale;
inline ConVar::ConVar *sv_tickms;

void SetupServerSideConvars();