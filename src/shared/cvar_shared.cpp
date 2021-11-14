#include "cvar_shared.hpp"

ConVar::CConVar *cvarLanguage = nullptr;

void SetupSharedConvars()
{
	cvarLanguage = conVarHandle.DeclareConvar( "language", "", ConVar::CVAR_ARCHIVE );
}