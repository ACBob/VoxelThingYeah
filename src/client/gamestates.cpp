#include "gamestates.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "packs.hpp"
#include "filesystem.hpp"
#include "rendering/texturemanager.hpp"

#include "rendering/modelloader.hpp"

#include "cvar_clientside.hpp"

#include <cstring>

void CStatePlay::Enter()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	m_projectionMatrix = glm::perspective( glm::radians( fov->GetFloat() ),
										   scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f );
	m_screenMatrix	   = glm::ortho( 0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat() );

	m_pDiffuseShader = shaderSystem::LoadShader( "generic.vert", "generic.frag" );
	m_pUnlitShader	 = shaderSystem::LoadShader( "generic.vert", "unlit.frag" );
	m_pSkyboxShader	 = shaderSystem::LoadShader( "sky.vert", "sky.frag" );
	m_pChunkShader	 = shaderSystem::LoadShader( "chunk.vert", "chunk.frag" );
	m_pWaterShader	 = shaderSystem::LoadShader( "water.vert", "generic.frag" );

	GetCubeModel( m_skyboxModel, CVector( -5, -5, -5 ) );
	m_pStellarModel = modelSystem::LoadModel( "sun.obj" );

	m_pStellarModel->SetShader( m_pUnlitShader );
	m_skyboxModel.SetShader( m_pSkyboxShader );

	m_pStellarModel->SetTexture( materialSystem::LoadTexture( "sun.png" ) );

	m_pTerrainPNG	   = materialSystem::LoadTexture( "terrain.png" );
	m_pHotbarTex	   = materialSystem::LoadTexture( "hotbar.png" );
	m_pHotbarSelectTex = materialSystem::LoadTexture( "hotbar-selected.png" );

	m_pLocalPlayer = new CEntityPlayer();
	m_pLocalWorld  = new CWorld( m_pChunkShader, m_pDiffuseShader, m_pWaterShader, m_pTerrainPNG );

	m_pLocalPlayer->m_pInputMan = pStateMan->m_pGui->m_pInputMan;
	m_pLocalPlayer->m_pClient	= pStateMan->m_pClient;

	m_pLocalWorld->AddEntity( m_pLocalPlayer );

	pStateMan->m_pClient->m_pLocalPlayer = m_pLocalPlayer;
	pStateMan->m_pClient->m_pLocalWorld	 = m_pLocalWorld;
	pStateMan->m_pClient->m_pParticleMan = &m_particleMan;
}
void CStatePlay::ReturnedTo() {}

void CStatePlay::Exit()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	delete m_pLocalPlayer;
	delete m_pLocalWorld;

	pStateMan->m_pClient->m_pLocalPlayer = nullptr;
	pStateMan->m_pClient->m_pLocalWorld	 = nullptr;
	pStateMan->m_pClient->m_pParticleMan = nullptr;
}

void CStatePlay::Update()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	pStateMan->m_pInputMan->m_bInGui = ( m_bInChat || m_bInPause || m_pLocalPlayer->m_bInInventory );

	char *guiBuf = new char[512];

	// Should we leave the game?
	bool leave = false;

	m_fSunAngle	= lerp(m_fSunAngle, 180 * ( 1 - ( m_pLocalWorld->m_iTimeOfDay / 12000.0f ) ), 1/60.0f);
	CVector vSunForward = CVector( 0, 1, 0 ).Rotate(1, cl_sunroll->GetFloat()).Rotate(2, cl_sunyaw->GetFloat()).Rotate( 3, m_fSunAngle );

	if ( !pStateMan->m_pClient->m_bConnected )
		leave = true;
	else
	{
		// -----------------------
		// Updating Stuff
		// -----------------------
		m_pLocalPlayer->UpdateClient( m_pLocalWorld, &m_particleMan );
		m_pLocalWorld->WorldTick( pStateMan->m_iTick, pStateMan->m_fDelta );

		m_particleMan.PhysicsTick( m_pLocalWorld, pStateMan->m_iTick, pStateMan->m_fDelta );

		if ( pStateMan->m_iTick != m_iLastTick )
		{
			// Bandwith patrol, come out with your hands up!
			protocol::SendClientPlayerPos( pStateMan->m_pClient->m_pPeer, m_pLocalPlayer->m_vPosition,
										   m_pLocalPlayer->m_vRotation );

			m_iLastTick = pStateMan->m_iTick;
		}

		soundSystem::SetListener( m_pLocalWorld, m_pLocalPlayer->m_vPosition, m_pLocalPlayer->GetForward(),
								  m_pLocalPlayer->m_vVelocity );

		// -----------------------
		// Rendering
		// -----------------------
		CVector f = m_pLocalPlayer->m_camera.GetForward();
		CVector v = m_pLocalPlayer->m_camera.m_vPosition + f;
		glm::mat4 view =
			glm::lookAt( glm::vec3( m_pLocalPlayer->m_camera.m_vPosition.x, m_pLocalPlayer->m_camera.m_vPosition.y,
									m_pLocalPlayer->m_camera.m_vPosition.z ),
						 glm::vec3( v.x, v.y, v.z ), glm::vec3( VEC_UP.x, VEC_UP.y, VEC_UP.z ) );
		shaderSystem::SetUniforms( view, m_projectionMatrix, m_iLastTick, m_pLocalWorld->m_iTimeOfDay, vSunForward );

		glDisable( GL_DEPTH_TEST ); // Skybox
		{
			m_skyboxModel.m_vPosition = m_pLocalPlayer->m_camera.m_vPosition;
			m_skyboxModel.Render();

			m_pStellarModel->m_vPosition = m_skyboxModel.m_vPosition;
			m_pStellarModel->m_vRotation = CVector( cl_sunroll->GetFloat(), cl_sunyaw->GetFloat(), -m_fSunAngle );
			m_pStellarModel->Render();
		}
		glEnable( GL_DEPTH_TEST );

		m_pDiffuseShader->Use();

		glBindTexture( GL_TEXTURE_2D, m_pTerrainPNG->m_iId );

		m_pLocalWorld->Render();

		// Particles Last
		m_particleMan.Render( m_pLocalPlayer->m_camera.m_vRotation );

		// m_particleMan.CreateParticle(m_pLocalPlayer->m_vPosition, CVector(2.5f - (rand() % 500) / 100.0f, 5.0f -
		// (rand() % 1000) / 100.0f, 2.5f - (rand() % 500) / 100.0f), {0,-.98,0}, 5, true);

		// -----------------------
		// Input
		// -----------------------
		if ( !m_bInPause && !m_bInChat )
		{
			if ( pStateMan->m_pInputMan->m_bInputState[INKEY_INV] &&
				 !pStateMan->m_pInputMan->m_bOldInputState[INKEY_INV] )
				m_pLocalPlayer->m_bInInventory = !m_pLocalPlayer->m_bInInventory;

			if ( pStateMan->m_pInputMan->m_bInputState[INKEY_CHAT] &&
				 !pStateMan->m_pInputMan->m_bOldInputState[INKEY_CHAT] )
				m_bInChat = !m_bInChat;
		}

		if ( pStateMan->m_pInputMan->m_bInputState[INKEY_OUT] && !pStateMan->m_pInputMan->m_bOldInputState[INKEY_OUT] )
		{
			if ( m_pLocalPlayer->m_bInInventory )
				m_pLocalPlayer->m_bInInventory = false;
			else if ( m_bInChat )
				m_bInChat = false;
			else
				m_bInPause = !m_bInPause;
		}

		// -----------------------
		// GUI
		// -----------------------
		pStateMan->m_pGui->Label( "Meegreef ALPHA", CVector( 0, -1 ) );

		pStateMan->m_pGui->Label( "\u263A Smiley!!!", CVector( 0, -2 ) );

		int hours	= m_pLocalWorld->m_iTimeOfDay / 1000;
		int minutes = ( m_pLocalWorld->m_iTimeOfDay - ( hours * 1000 ) ) / 16.6666;
		snprintf( guiBuf, 100, "%02i:%02i", hours, minutes );
		pStateMan->m_pGui->Label( guiBuf, CVector( pStateMan->m_pGui->m_vScreenCentre.x, -1 ), Color( 1, 1, 1 ),
								  CGui::TEXTALIGN_CENTER );

		pStateMan->m_pGui->Image( m_pHotbarTex, CVector( pStateMan->m_pGui->m_vScreenCentre.x, 0 ),
								  CVector( 18.5, 2.5 ), CVector( 0.5, 0 ) );
		float p;

		for ( int i = 0; i < 8; i++ )
		{
			p = 8.0f * ( i / 4.0f - 1.0f );

			if ( m_pLocalPlayer->m_inventory.Slot( i ) == nullptr ||
				 m_pLocalPlayer->m_inventory.Slot( i )->GetCount() == 0 )
				continue;

			BlockTexture bTex = GetDefaultBlockTextureSide(
				reinterpret_cast<CBlockItem *>( m_pLocalPlayer->m_inventory.Slot( i ) )->m_iBlockType,
				Direction::NORTH );
			pStateMan->m_pGui->ImageAtlas(
				m_pTerrainPNG, { (float)bTex.x, 15.0f - (float)bTex.y, (float)bTex.sizex, (float)bTex.sizey }, 16.0f,
				CVector( p + pStateMan->m_pGui->m_vScreenCentre.x, 1.25 ), CVector( 2, 2 ), CVector( 0.5, 0.5 ) );

			snprintf( guiBuf, 100, "%d", m_pLocalPlayer->m_inventory.Slot( i )->GetCount() );
			pStateMan->m_pGui->Label( guiBuf, CVector( p + pStateMan->m_pGui->m_vScreenCentre.x, 0 ) );
		}
		p = 8.0f * ( m_pLocalPlayer->m_iSelectedItemIDX / 4.0f - 1.0f );
		pStateMan->m_pGui->Image( m_pHotbarSelectTex, CVector( p + pStateMan->m_pGui->m_vScreenCentre.x, 1.25 ),
								  CVector( 3, 3 ), CVector( 0.5, 0.5 ) );

		if ( m_pLocalPlayer->m_bInInventory )
		{
			pStateMan->m_pGui->Image( pStateMan->m_pGui->m_pInventoryTex, pStateMan->m_pGui->m_vScreenCentre,
									  CVector( 22, 22 ), CVector( 0.5, 0.5 ) );
			pStateMan->m_pGui->Label( "Inventory...", pStateMan->m_pGui->m_vScreenCentre + CVector( -9.625, 9.625 ) );

			CVector p  = pStateMan->m_pGui->m_vScreenCentre + CVector( -8, 8 );
			CVector op = p;
			int j	   = 0;
			for ( int i = blocktype_t::STONE; i <= blocktype_t::WOOL; i++ )
			{
				// Skip useless liquid flow variants
				if ( i == WATER || i == LAVA )
					continue;
				j++;

				BlockTexture bTex = GetDefaultBlockTextureSide( (blocktype_t)i, Direction::NORTH );
				if ( pStateMan->m_pGui->AtlasButton(
						 'b' + i, m_pTerrainPNG,
						 { (float)bTex.x, 15.0f - (float)bTex.y, (float)bTex.sizex, (float)bTex.sizey }, 16.0f, p,
						 CVector( 2, 2 ), CVector( 0.5, 0.5 ) ) )
				{
					if ( m_pLocalPlayer->m_pSelectedItem != nullptr )
					{
						// TODO: assuming blockitem
						m_pLocalPlayer->m_pSelectedItem->SetCount( ITEMSTACK_MAX );
						reinterpret_cast<CBlockItem *>( m_pLocalPlayer->m_pSelectedItem )->m_iBlockType =
							(blocktype_t)i;
					}
				}

				p.x += 2;
				if ( j % 9 == 0 )
				{
					p.y -= 2;
					p.x = op.x;
				}
			}
		}
		else if ( m_bInPause )
		{
			if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Back to Game",
												 pStateMan->m_pGui->m_vScreenCentre + CVector( 0, 4 ),
												 CVector( 0.5, 0.5 ), CVector( 2, 1 ), CVector( 14, 2 ) ) )
			{
				m_bInPause = false;
			}
			if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Options", pStateMan->m_pGui->m_vScreenCentre,
												 CVector( 0.5, 0.5 ), CVector( 2, 1 ), CVector( 14, 2 ) ) )
			{
				m_pStateMan->PushState( std::make_unique<CStateOptionsMenu>() );
			}
			if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Disconnect",
												 pStateMan->m_pGui->m_vScreenCentre - CVector( 0, 4 ),
												 CVector( 0.5, 0.5 ), CVector( 2, 1 ), CVector( 14, 2 ) ) )
			{
				pStateMan->m_pClient->Disconnect();
				pStateMan->PopState();
			}
		}
		else
		{
			pStateMan->m_pGui->Crosshair();
		}

		// Chat Rendering
		for ( int i = 5; i > 0; i-- )
		{
			if ( i > pStateMan->m_pClient->m_chatBuffer.size() )
				continue;

			pStateMan->m_pGui->Label(
				pStateMan->m_pClient->m_chatBuffer[pStateMan->m_pClient->m_chatBuffer.size() - i].c_str(),
				CVector( 0, i + 1 ) );
		}

		if ( m_bInChat )
		{
			// Chat
			const char *chat = pStateMan->m_pGui->TextInput( 69, CVector( 0, 0 ) );

			if ( chat != nullptr )
			{
				protocol::SendClientChatMessage( pStateMan->m_pClient->m_pPeer, chat );

				pStateMan->m_pGui->SetTextBuffer(69, "");

				m_bInChat = false;
			}
		}
	}

	delete[] guiBuf;

	if ( leave )
		m_pStateMan->PopState();
}

void CStateMenu::Enter()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	// 3 and 4 are used for ip and port respectively
	pStateMan->m_pGui->SetTextBuffer( 3, cl_ip->GetString() );
	pStateMan->m_pGui->SetTextBuffer( 4, cl_port->GetString() );
	pStateMan->m_pGui->SetTextBuffer( 5, username->GetString() );
}

void CStateMenu::ReturnedTo()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	// 3 and 4 are used for ip and port respectively
	pStateMan->m_pGui->SetTextBuffer( 3, cl_ip->GetString() );
	pStateMan->m_pGui->SetTextBuffer( 4, cl_port->GetString() );
	pStateMan->m_pGui->SetTextBuffer( 5, username->GetString() );

	// If there's a kick reason, it's safe to assume we've been kicked. In such case, display the kick screen.
	if ( strlen( cl_kickreason->GetString() ) )
	{
		pStateMan->PushState( std::make_unique<CKickScreen>() );
	}
}

void CStateMenu::Exit()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	pStateMan->m_pClient->Disconnect();
}

void CStateMenu::Update()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	pStateMan->m_pInputMan->m_bInGui = true;

	pStateMan->m_pGui->Image( pStateMan->m_pGui->m_pBGTex, CVector( 0, 0 ),
							  pStateMan->m_pGui->m_vScreenDimensions / pStateMan->m_pGui->m_iGuiUnit, CVector( 0, 0 ),
							  CVector( 0.5, 0.5, 0.5 ) );

	pStateMan->m_pGui->Image( pStateMan->m_pGui->m_pLogoTex, CVector( pStateMan->m_pGui->m_vScreenCentre.x, -1 ),
							  CVector( 10 * 5.25, 10 ), CVector( 0.5, 1 ) );

	if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Play", pStateMan->m_pGui->m_vScreenCentre, CVector( 0.5, 0.5 ),
										 CVector( 2, 1 ), CVector( 14, 2 ) ) )
	{
		cl_ip->SetString( pStateMan->m_pGui->GetTextBuffer( 3 ) );
		cl_port->SetString( pStateMan->m_pGui->GetTextBuffer( 4 ) );
		username->SetString( pStateMan->m_pGui->GetTextBuffer( 5 ) );

		pStateMan->m_pClient->Connect( cl_ip->GetString(), cl_port->GetInt() );
		m_pStateMan->PushState( std::make_unique<CStatePlay>() );
	}

	if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Options", pStateMan->m_pGui->m_vScreenCentre - CVector( 0, 2 ),
										 CVector( 0.5, 0.5 ), CVector( 2, 1 ), CVector( 14, 2 ) ) )
	{
		cl_ip->SetString( pStateMan->m_pGui->GetTextBuffer( 3 ) );
		cl_port->SetString( pStateMan->m_pGui->GetTextBuffer( 4 ) );
		username->SetString( pStateMan->m_pGui->GetTextBuffer( 5 ) );
		m_pStateMan->PushState( std::make_unique<CStateOptionsMenu>() );
	}
	if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Quit", pStateMan->m_pGui->m_vScreenCentre - CVector( 0, 4 ),
										 CVector( 0.5, 0.5 ), CVector( 2, 1 ), CVector( 14, 2 ) ) )
	{
		pStateMan->PopState();
	}

	pStateMan->m_pGui->Label( "IP:", pStateMan->m_pGui->m_vScreenCentre - CVector( 16, 4.5 ), Color( 1, 1, 1 ),
							  CGui::TEXTALIGN_CENTER );
	pStateMan->m_pGui->SelectableTextInput( 3, pStateMan->m_pGui->m_vScreenCentre - CVector( 24, 7 ),
											CVector( 16, 2 ) );
	pStateMan->m_pGui->Label( "Port:", pStateMan->m_pGui->m_vScreenCentre - CVector( 16, 9.5 ), Color( 1, 1, 1 ),
							  CGui::TEXTALIGN_CENTER );
	pStateMan->m_pGui->SelectableTextInput( 4, pStateMan->m_pGui->m_vScreenCentre - CVector( 24, 12 ),
											CVector( 16, 2 ) );
	pStateMan->m_pGui->Label( "Username:", pStateMan->m_pGui->m_vScreenCentre - CVector( -16, 4.5 ), Color( 1, 1, 1 ),
							  CGui::TEXTALIGN_CENTER );
	pStateMan->m_pGui->SelectableTextInput( 5, pStateMan->m_pGui->m_vScreenCentre - CVector( -8, 7 ),
											CVector( 16, 2 ) );
}

void CStateOptionsMenu::Enter()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	m_iVolumeSlider = cl_volume->GetFloat() * 100;
	m_bEnableReverb = cl_reverb->GetBool();
}
void CStateOptionsMenu::ReturnedTo() {}
void CStateOptionsMenu::Exit()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();
}
void CStateOptionsMenu::Update()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	soundSystem::SetListener( nullptr, CVector( 0, 0, 0 ), CVector( 0, 0, 1 ), CVector( 0, 0, 0 ) );

	pStateMan->m_pInputMan->m_bInGui = true;

	pStateMan->m_pGui->Image( pStateMan->m_pGui->m_pBGTex, CVector( 0, 0 ),
							  pStateMan->m_pGui->m_vScreenDimensions / pStateMan->m_pGui->m_iGuiUnit, CVector( 0, 0 ),
							  CVector( 0.5, 0.5, 0.5 ) );

	pStateMan->m_pGui->Label( "Options...", CVector( pStateMan->m_pGui->m_vScreenCentre.x, -2 ), Color( 1, 1, 1 ),
							  CGui::TEXTALIGN_CENTER );

	if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Return", CVector( pStateMan->m_pGui->m_vScreenCentre.x, 2 ),
										 CVector( 0.5, 0.5 ), CVector( 2, 1 ), CVector( 14, 2 ) ) )
	{
		m_pStateMan->PopState();
	}

	if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Resource Packs", CVector( pStateMan->m_pGui->m_vScreenCentre.x, 8 ),
										 CVector( 0.5, 0.5 ), CVector( 2, 1 ), CVector( 14, 2 ) ) )
	{
		m_pStateMan->PushState(std::make_unique<CStatePackMenu>());
	}

	pStateMan->m_pGui->Label( "Master Volume Slider", pStateMan->m_pGui->m_vScreenCentre + CVector( -16, 6 ) );
	if ( pStateMan->m_pGui->HorzSlider( GUIGEN_ID, pStateMan->m_pGui->m_vScreenCentre + CVector( -16, 4 ),
										CVector( 32, 2 ), 100, m_iVolumeSlider ) )
	{
		cl_volume->SetFloat( m_iVolumeSlider / 100.0f );
	}
	pStateMan->m_pGui->Label( "Enable Reverb", pStateMan->m_pGui->m_vScreenCentre + CVector( -16, 2 ) );
	if ( pStateMan->m_pGui->CheckBox( GUIGEN_ID, pStateMan->m_pGui->m_vScreenCentre + CVector( -16, 0 ),
									  CVector( 2, 2 ), m_bEnableReverb ) )
	{
		cl_reverb->SetBool( m_bEnableReverb );
	}
}

void CKickScreen::Enter()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();
}
void CKickScreen::ReturnedTo() {}
void CKickScreen::Exit()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	cl_kickreason->SetString( "" );
}
void CKickScreen::Update()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	pStateMan->m_pInputMan->m_bInGui = true;

	pStateMan->m_pGui->Image( pStateMan->m_pGui->m_pBGTex, CVector( 0, 0 ),
							  pStateMan->m_pGui->m_vScreenDimensions / pStateMan->m_pGui->m_iGuiUnit, CVector( 0, 0 ),
							  CVector( 0.5, 0.5, 0.5 ) );

	pStateMan->m_pGui->Label( "Kicked!", CVector( pStateMan->m_pGui->m_vScreenCentre.x, -2 ), Color( 1, 1, 1 ),
							  CGui::TEXTALIGN_CENTER );

	if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Back", CVector( pStateMan->m_pGui->m_vScreenCentre.x, 2 ),
										 CVector( 0.5, 0.5 ) ) )
	{
		m_pStateMan->PopState();
	}

	pStateMan->m_pGui->Label( cl_kickreason->GetString(), pStateMan->m_pGui->m_vScreenCentre, Color( 1, 0.5, 0.5 ),
							  CGui::TEXTALIGN_CENTER );
}


void CStatePackMenu::Enter()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	// List resource packs
	m_packList = resourcePacks::ListPacks();
	m_packEnabled.resize(m_packList.size(), false);
}
void CStatePackMenu::ReturnedTo() {}
void CStatePackMenu::Exit()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	// Remount everything!
	fileSystem::UnMountAll();
	fileSystem::Mount("files", "/", true);

	cl_resourcepacks->SetString("");

	for (int i = 0; i < m_packList.size(); i++)
	{
		if (m_packEnabled[i])
		{
			const char *c = cl_resourcepacks->GetString();
			char *nc = new char[strlen(c) + 2 + m_packList[i].internalName.size()];
			strcpy(nc, c);
			strcat(nc, ",");
			strcat(nc, m_packList[i].internalName.c_str());

			cl_resourcepacks->SetString(nc);

			// Mount it
			fileSystem::Mount(("files" + m_packList[i].path + "/assets").c_str(), "/assets", true);
		}
	}

	// Now reload all assets!
	materialSystem::ReloadTextures();
}
void CStatePackMenu::Update()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	pStateMan->m_pInputMan->m_bInGui = true;

	pStateMan->m_pGui->Image( pStateMan->m_pGui->m_pBGTex, CVector( 0, 0 ),
							  pStateMan->m_pGui->m_vScreenDimensions / pStateMan->m_pGui->m_iGuiUnit, CVector( 0, 0 ),
							  CVector( 0.5, 0.5, 0.5 ) );

	pStateMan->m_pGui->Label( "Resource Packs...", CVector( pStateMan->m_pGui->m_vScreenCentre.x, -2 ), Color( 1, 1, 1 ),
							  CGui::TEXTALIGN_CENTER );

	if ( pStateMan->m_pGui->LabelButton( GUIGEN_ID, "Back", CVector( pStateMan->m_pGui->m_vScreenCentre.x, 2 ),
										 CVector( 0.5, 0.5 ) ) )
	{
		m_pStateMan->PopState();
	}


	// TODO: fix this to be scrollable
	int y = 0;
	for (resourcePacks::packInfo &pack : m_packList)
	{
		pStateMan->m_pGui->Image(pStateMan->m_pGui->m_pPackPNG, CVector(1, 24 - y * 4), CVector(4,4), CVector(0, 0.5));
		pStateMan->m_pGui->Label(pack.name.c_str(), CVector(5,  24 - y * 4));
		bool enable = m_packEnabled[y];
		if (pStateMan->m_pGui->CheckBox(GUIGEN_ID + y, CVector(5, 22 - y * 4), CVector(2,2), enable))
		{
			m_packEnabled[y] = enable;
		} 

		y = y + 1;
	}
}