#include "gui.hpp"
#include "guistateplay.hpp"
#include "guistatemanager.hpp" // For returning a different state if any

#include "cvar_clientside.hpp"

#include "network/protocol.hpp"

#include "entities/entityplayer.hpp"

CGuiStatePlay::CGuiStatePlay()
{
	m_iState = STATE_NORMAL;
}

unsigned int CGuiStatePlay::Frame(CGui *pGui, CWorld *pLocalWorld)
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
			// protocol::SendClientChatMessage( client.m_pPeer, chat );
			// chatting		  = false;
			pGui->m_pInputMan->m_bInGui = false;
		}
	}

	// for ( int i = 0; i < 5; i++ )
	// {
	// 	int j = client.m_chatBuffer.size() - i;
	// 	if ( j < 0 || j >= client.m_chatBuffer.size() )
	// 		continue;

	// 	std::string msg = client.m_chatBuffer.at( j );

	// 	pGui->Label( msg.c_str(), CVector( 0, 2 + i ) );
	// }

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
	// pGui->ImageAtlas( terrainPng, { (float)bTex.x, 15.0f - (float)bTex.y, (float)bTex.sizex, (float)bTex.sizey },
	// 				16, CVector( -1, -1 ), CVector( 4, 4 ), CVector( 1, 1 ) );

	// pGui->Image( crosshairTex, gui.m_vScreenCentre, CVector( 2, 2 ), CVector( 0.5, 0.5 ) ); // TODO: lmao
	// pGui->Update();

	delete[] buf;

	return CGuiStateManager::GUISTATE_PLAY;
}