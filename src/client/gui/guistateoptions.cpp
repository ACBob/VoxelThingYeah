#include "guistateoptions.hpp"
#include "gui.hpp"
#include "guistatemanager.hpp" // For returning a different state if any

#include "cvar_clientside.hpp"

#include "network/protocol.hpp"

#include "entities/entityplayer.hpp"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

CGuiStateOptions::CGuiStateOptions() { m_iState = STATE_NORMAL; }

unsigned int CGuiStateOptions::Frame( CGui *pGui, CWorld *pLocalWorld )
{
	if ( pGui->LabelButton( 1, "Back", pGui->m_vScreenCentre + CVector( 0, -1.5 ), CVector( 0.5, 0 ) ) )
	{
		con_info( "Pizza Pasta" );
		return CGuiStateManager::GUISTATE_PLAY;
	}

	return CGuiStateManager::GUISTATE_OPTIONS;
}