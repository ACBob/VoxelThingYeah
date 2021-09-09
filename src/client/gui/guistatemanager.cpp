#include "guistatemanager.hpp"

#include "cvar_clientside.hpp"

CGuiStateManager::CGuiStateManager() {}
CGuiStateManager::~CGuiStateManager() {}

void CGuiStateManager::Update(CEntityPlayer *pLocalPlayer, CWorld *pLocalWorld)
{
	switch (m_state)
	{
		case GUISTATE_PLAY:
		{

			switch(m_subState)
			{
				case PLAY_CHAT:
				case PLAY_NONE:
				{
					char *buf = new char[100];
					snprintf( buf, 100, "Connected to \"%s\"", cl_servername->GetString() );
					m_pGui->Label( buf, CVector( 0, 0 ) );

					m_pGui->Label( "Bobcraft", CVector( 0, 1 ) );

					// if ( chatting )
					// {
					// 	const char *chat = m_pGui->TextInput( 0, CVector( 0, 2 ) );
					// 	if ( chat != nullptr )
					// 	{
					// 		protocol::SendClientChatMessage( client.m_pPeer, chat );
					// 		chatting		  = false;
					// 		inputMan.m_bInGui = false;
					// 	}
					// }

					// for ( int i = 0; i < 5; i++ )
					// {
					// 	int j = client.m_chatBuffer.size() - i;
					// 	if ( j < 0 || j >= client.m_chatBuffer.size() )
					// 		continue;

					// 	std::string msg = client.m_chatBuffer.at( j );

					// 	m_pGui->Label( msg.c_str(), CVector( 0, 2 + i ) );
					// }

					snprintf( buf, 100, "<%.2f,%.2f,%.2f>", pLocalPlayer->m_vPosition.x, pLocalPlayer->m_vPosition.y, pLocalPlayer->m_vPosition.z );
					m_pGui->Label( buf, CVector( 0, -1 ) );
					int hours	= pLocalWorld->m_iTimeOfDay / 1000;
					int minutes = ( pLocalWorld->m_iTimeOfDay - ( hours * 1000 ) ) / 16.6666;
					snprintf( buf, 100, "Time %02i:%02i", hours, minutes );
					m_pGui->Label( buf, CVector( 0, -2 ) );

					BlockTexture bTex = GetDefaultBlockTextureSide( pLocalPlayer->m_iSelectedBlockType, Direction::NORTH );
					// m_pGui->ImageAtlas( terrainPng, { (float)bTex.x, 15.0f - (float)bTex.y, (float)bTex.sizex, (float)bTex.sizey },
					// 				16, CVector( -1, -1 ), CVector( 4, 4 ), CVector( 1, 1 ) );

					// m_pGui->Image( crosshairTex, gui.m_vScreenCentre, CVector( 2, 2 ), CVector( 0.5, 0.5 ) ); // TODO: lmao
					m_pGui->Update();

					delete[] buf;
				}
				break;

				case PLAY_PAUSE:
				break;
			}
		}
		break;
	}

	m_pGui->Update();
}