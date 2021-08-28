// Client-side ConVars.

#pragma once

#include "shared/cvar.hpp"

extern ConVar::ConVar *fov;

extern ConVar::ConVar *scr_width;
extern ConVar::ConVar *scr_height;

extern ConVar::ConVar *username;

extern ConVar::ConVar *cl_ip;
extern ConVar::ConVar *cl_port;

void SetupClientSideConvars();