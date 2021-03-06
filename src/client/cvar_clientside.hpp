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
extern ConVar::CConVar *cl_reverb;
extern ConVar::CConVar *cl_mufflesounds;

// extern ConVar::CConVar *playerskin;

extern ConVar::CConVar *cl_kickreason;

extern ConVar::CConVar *cl_resourcepacks;

extern ConVar::CConVar *cl_dodaylightcycle;
extern ConVar::CConVar *cl_sunroll;
extern ConVar::CConVar *cl_sunyaw;

extern ConVar::CConVar *cl_maxfps;

void SetupClientSideConvars();