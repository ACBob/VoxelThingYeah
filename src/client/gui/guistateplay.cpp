#include "gui.hpp"
#include "guistateplay.hpp"
#include "guistatemanager.hpp" // For returning a different state if any

#include "cvar_clientside.hpp"

#include "network/protocol.hpp"

#include "entities/entityplayer.hpp"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

CGuiStatePlay::CGuiStatePlay()
{
	m_iState = STATE_NORMAL;

	m_pCrossHairTex = materialSystem::LoadTexture("crosshair.png");
	m_pTerrainTex = materialSystem::LoadTexture("terrain.png");
}

unsigned int CGuiStatePlay::Frame(CGui *pGui, CWorld *pLocalWorld)
{
	if ( m_iState == STATE_NORMAL && pGui->m_pInputMan->m_bInputState[INKEY_CHAT] && !pGui->m_pInputMan->m_bOldInputState[INKEY_CHAT] )
		m_iState = STATE_CHATTING;
	else if ( pGui->m_pInputMan->m_bInputState[INKEY_OUT] && !pGui->m_pInputMan->m_bOldInputState[INKEY_OUT] )
	{
		if ( m_iState == STATE_CHATTING )
			m_iState = STATE_NORMAL;
		else if (m_iState == STATE_NORMAL)
			m_iState = STATE_PAUSED;
		else if (m_iState == STATE_PAUSED)
			m_iState = STATE_NORMAL;
	}

	pGui->m_pInputMan->m_bInGui = !(m_iState == STATE_NORMAL);

	if (m_iState == STATE_PAUSED)
	{
		pGui->Label("GAME PAUSED", pGui->m_vScreenCentre + CVector(0,5), CVector(1,1,1), CGui::TEXTALIGN_CENTER);

		if (pGui->LabelButton(1, "Back to Game", pGui->m_vScreenCentre + CVector(0,1), CVector(0.5, 0)))
			m_iState = STATE_NORMAL;
		if (pGui->LabelButton(1, "Options", pGui->m_vScreenCentre + CVector(0,-1.5), CVector(0.5, 0)))
			return CGuiStateManager::GUISTATE_PLAY;
	}
	else
	{
		char *buf = new char[100];
		snprintf( buf, 100, "Connected to \"%s\"", cl_servername->GetString() );
		pGui->Label( buf, CVector( 0, 0 ) );

		pGui->Label( "Bobcraft", CVector( 0, 1 ) );		

		if ( m_iState == STATE_CHATTING )
		{
			const char *chat = pGui->TextInput( 0, CVector( 0, 2 ) );
			if ( chat != nullptr )
			{
				protocol::SendClientChatMessage( pGui->m_pClient->m_pPeer, chat );
				m_iState = STATE_NORMAL;
				pGui->m_pInputMan->m_bInGui = false;
			}
		}

		for ( int i = 0; i < 5; i++ )
		{
			int j = pGui->m_pClient->m_chatBuffer.size() - i;
			if ( j < 0 || j >= pGui->m_pClient->m_chatBuffer.size() )
				continue;

			std::string msg = pGui->m_pClient->m_chatBuffer.at( j );

			pGui->Label( msg.c_str(), CVector( 0, 2 + i ) );
		}

		snprintf( buf, 100, "<%.2f,%.2f,%.2f>",	
			reinterpret_cast<CEntityPlayer*>(pLocalWorld->m_pLocalPlayer)->m_vPosition.x,
			reinterpret_cast<CEntityPlayer*>(pLocalWorld->m_pLocalPlayer)->m_vPosition.y,
			reinterpret_cast<CEntityPlayer*>(pLocalWorld->m_pLocalPlayer)->m_vPosition.z );
		pGui->Label( buf, CVector( 0, -1 ) );
		int hours	= pLocalWorld->m_iTimeOfDay / 1000;
		int minutes = ( pLocalWorld->m_iTimeOfDay - ( hours * 1000 ) ) / 16.6666;
		snprintf( buf, 100, "Time %02i:%02i", hours, minutes );
		pGui->Label( buf, CVector( 0, -2 ) );

		BlockTexture bTex = GetDefaultBlockTextureSide( reinterpret_cast<CEntityPlayer*>(pLocalWorld->m_pLocalPlayer)->m_iSelectedBlockType, Direction::NORTH );
		pGui->ImageAtlas( m_pTerrainTex, { (float)bTex.x, 15.0f - (float)bTex.y, (float)bTex.sizex, (float)bTex.sizey },
						16, CVector( -1, -1 ), CVector( 4, 4 ), CVector( 1, 1 ) );

		pGui->Image( m_pCrossHairTex, pGui->m_vScreenCentre, CVector( 2, 2 ), CVector( 0.5, 0.5 ) );
		// pGui->Update();

		delete[] buf;
	}

	return CGuiStateManager::GUISTATE_PLAY;
}