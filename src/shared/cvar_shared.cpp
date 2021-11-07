#include "cvar_shared.hpp"

ConVar::CConVar *cvarLanguage = nullptr;

void SetupSharedConvars()
{
	cvarLanguage = conVarHandle.DeclareConvar( "language", "en", ConVar::CVAR_ARCHIVE );
}