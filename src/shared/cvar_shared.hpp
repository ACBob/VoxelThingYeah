// Shared CVars, used by both the client and server.

#pragma once

#include "cvar.hpp"

extern ConVar::CConVar *cvarLanguage;

void SetupSharedConvars();