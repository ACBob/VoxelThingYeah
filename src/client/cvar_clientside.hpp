// Client-side ConVars.

#pragma once

#include "shared/cvar.hpp"

extern ConVar::CConVar *fov;

extern ConVar::CConVar *scr_width;
extern ConVar::CConVar *scr_height;

extern ConVar::CConVar *username;

extern ConVar::CConVar *cl_ip;
extern ConVar::CConVar *cl_port;

extern ConVar::CConVar *cl_servername;

extern ConVar::CConVar *cl_volume;

void SetupClientSideConvars();