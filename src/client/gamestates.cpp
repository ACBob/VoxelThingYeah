#include "gamestates.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rendering/modelloader.hpp"

#include "cvar_clientside.hpp"

void CStatePlay::Enter()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	m_projectionMatrix = glm::perspective( glm::radians( fov->GetFloat() ),
										   scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f );
	m_screenMatrix	   = glm::ortho( 0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat() );

	m_pDiffuseShader = shaderSystem::LoadShader( "shaders/generic.vert", "shaders/generic.frag" );
	m_pUnlitShader	 = shaderSystem::LoadShader( "shaders/generic.vert", "shaders/unlit.frag" );
	m_pSkyboxShader	 = shaderSystem::LoadShader( "shaders/sky.vert", "shaders/sky.frag" );
	m_pWaterShader	 = shaderSystem::LoadShader( "shaders/water.vert", "shaders/generic.frag" );

	GetCubeModel( m_skyboxModel, CVector( -5, -5, -5 ) );
	m_pStellarModel = modelSystem::LoadModel( "models/sun.obj" );

	m_pStellarModel->SetShader( m_pUnlitShader );
	m_skyboxModel.SetShader( m_pSkyboxShader );

	m_pStellarModel->SetTexture( materialSystem::LoadTexture( "sun.png" ) );

	m_pTerrainPNG = materialSystem::LoadTexture( "terrain.png" );
	m_pHotbarTex = materialSystem::LoadTexture( "hotbar.png" );
	m_pHotbarSelectTex = materialSystem::LoadTexture( "hotbar-selected.png" );

	m_pLocalPlayer = new CEntityPlayer();
	m_pLocalWorld  = new CWorld( m_pDiffuseShader, m_pDiffuseShader, m_pWaterShader, m_pTerrainPNG );

	m_pLocalPlayer->m_pInputMan = pStateMan->m_pGui->m_pInputMan;
	m_pLocalPlayer->m_pClient	= pStateMan->m_pClient;

	m_pLocalWorld->AddEntity( m_pLocalPlayer );

	pStateMan->m_pClient->m_pLocalPlayer = m_pLocalPlayer;
	pStateMan->m_pClient->m_pLocalWorld	 = m_pLocalWorld;
}

void CStatePlay::Exit()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	delete m_pLocalPlayer;
	delete m_pLocalWorld;

	pStateMan->m_pClient->m_pLocalPlayer = nullptr;
	pStateMan->m_pClient->m_pLocalWorld	 = nullptr;
}

void CStatePlay::Update()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	pStateMan->m_pInputMan->m_bInGui = ( m_bInPause || m_pLocalPlayer->m_bInInventory );

	char *guiBuf = new char[512];

	// Should we leave the game?
	bool leave = false;

	float fSunAngle		= 180 * ( 1 - ( m_pLocalWorld->m_iTimeOfDay / 12000.0f ) );
	CVector vSunForward = CVector( 0, 1, 0 ).Rotate( 3, fSunAngle );

	if ( !pStateMan->m_pClient->m_bConnected )
		leave = true;
	else
	{
		// -----------------------
		// Updating Stuff
		// -----------------------
		m_pLocalPlayer->UpdateClient( m_pLocalWorld );
		m_pLocalWorld->WorldTick( pStateMan->m_iTick, pStateMan->m_fDelta );

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
			m_pStellarModel->m_vRotation = CVector( 0, 0, -fSunAngle );
			m_pStellarModel->Render();
		}
		glEnable( GL_DEPTH_TEST );

		m_pDiffuseShader->Use();

		glBindTexture( GL_TEXTURE_2D, m_pTerrainPNG->m_iId );

		m_pLocalWorld->Render();

		if ( !m_bInPause && pStateMan->m_pInputMan->m_bInputState[INKEY_INV] &&
			 !pStateMan->m_pInputMan->m_bOldInputState[INKEY_INV] )
			m_pLocalPlayer->m_bInInventory = !m_pLocalPlayer->m_bInInventory;

		if ( pStateMan->m_pInputMan->m_bInputState[INKEY_OUT] && !pStateMan->m_pInputMan->m_bOldInputState[INKEY_OUT] )
		{
			if ( m_pLocalPlayer->m_bInInventory )
				m_pLocalPlayer->m_bInInventory = false;
			else
				m_bInPause = !m_bInPause;
		}

		// -----------------------
		// GUI
		// -----------------------
		pStateMan->m_pGui->Label( "Meegreef ALPHA", CVector( 0, -1 ) );

		int hours	= m_pLocalWorld->m_iTimeOfDay / 1000;
		int minutes = ( m_pLocalWorld->m_iTimeOfDay - ( hours * 1000 ) ) / 16.6666;
		snprintf( guiBuf, 100, "%02i:%02i", hours, minutes );
		pStateMan->m_pGui->Label( guiBuf, CVector( pStateMan->m_pGui->m_vScreenCentre.x, -1 ), Color( 1, 1, 1 ),
								  CGui::TEXTALIGN_CENTER );

		pStateMan->m_pGui->Image(m_pHotbarTex, CVector(pStateMan->m_pGui->m_vScreenCentre.x, 0), CVector(18.5, 2.5), CVector(0.5, 0));
		float p;
		
		for (int i = 0; i < 8; i++)
		{
			p = 8.0f * (i / 4.0f -1.0f);

			if (m_pLocalPlayer->m_inventory.Slot(i) == nullptr || m_pLocalPlayer->m_inventory.Slot(i)->GetCount() == 0)
				continue;
				
			BlockTexture bTex = GetDefaultBlockTextureSide( reinterpret_cast<CBlockItem*>(m_pLocalPlayer->m_inventory.Slot(i))->m_iBlockType, Direction::NORTH );
			pStateMan->m_pGui->ImageAtlas( m_pTerrainPNG,
										{ (float)bTex.x, 15.0f - (float)bTex.y, (float)bTex.sizex, (float)bTex.sizey },
										16.0f, CVector(p + pStateMan->m_pGui->m_vScreenCentre.x, 1.25), CVector( 2, 2 ), CVector( 0.5, 0.5 ) );
			
			snprintf( guiBuf, 100, "%d", m_pLocalPlayer->m_inventory.Slot(i)->GetCount() );
			pStateMan->m_pGui->Label(guiBuf, CVector(p + pStateMan->m_pGui->m_vScreenCentre.x, 0));
		}
		p = 8.0f * (m_pLocalPlayer->m_iSelectedItemIDX / 4.0f -1.0f);
		pStateMan->m_pGui->Image(m_pHotbarSelectTex, CVector(p + pStateMan->m_pGui->m_vScreenCentre.x, 1.25), CVector(3,3), CVector(0.5, 0.5));

		if ( m_pLocalPlayer->m_bInInventory )
		{
			pStateMan->m_pGui->Image( pStateMan->m_pGui->m_pInventoryTex, pStateMan->m_pGui->m_vScreenCentre,
									  CVector( 20, 20 ), CVector( 0.5, 0.5 ) );

			CVector p  = pStateMan->m_pGui->m_vScreenCentre + CVector( -8, 5.5 );
			CVector op = p;
			for ( int i = blocktype_t::STONE; i <= blocktype_t::ICE; i++ )
			{
				snprintf( guiBuf, 100, "%d", i );
				BlockTexture bTex = GetDefaultBlockTextureSide( (blocktype_t)i, Direction::NORTH );
				if ( pStateMan->m_pGui->AtlasButton(
						 'b' + i, m_pTerrainPNG,
						 { (float)bTex.x, 15.0f - (float)bTex.y, (float)bTex.sizex, (float)bTex.sizey }, 16.0f, p,
						 CVector( 2, 2 ) ) )
				{
					if (m_pLocalPlayer->m_pSelectedItem != nullptr)
					{
						// TODO: assuming blockitem
						m_pLocalPlayer->m_pSelectedItem->SetCount(ITEMSTACK_MAX);
						reinterpret_cast<CBlockItem*>(m_pLocalPlayer->m_pSelectedItem)->m_iBlockType = (blocktype_t)i;
					}
				}

				p.x += 2;
				if ( i % 8 == 0 )
				{
					p.y -= 2;
					p.x = op.x;
				}
			}
		}
		else
		{
			pStateMan->m_pGui->Crosshair();
		}
	}

	delete[] guiBuf;

	if ( leave )
		m_pStateMan->PopState();
}

void CStateMenu::Enter() {}

void CStateMenu::Exit()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	pStateMan->m_pClient->Disconnect();
}

void CStateMenu::Update()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );

	pStateMan->m_pInputMan->m_bInGui = true;

	pStateMan->m_pGui->Image( pStateMan->m_pGui->m_pBGTex, CVector( 0, 0 ),
							  pStateMan->m_pGui->m_vScreenDimensions / pStateMan->m_pGui->m_iGuiUnit, CVector( 0, 0 ),
							  CVector( 0.5, 0.5, 0.5 ) );

	pStateMan->m_pGui->Image( pStateMan->m_pGui->m_pLogoTex, CVector(pStateMan->m_pGui->m_vScreenCentre.x, -1), CVector(10 * 5.25,10), CVector(0.5, 1) );

	if ( pStateMan->m_pGui->LabelButton( 1, "Play", pStateMan->m_pGui->m_vScreenCentre, CVector( 0.5, 0.5 ) ) )
	{
		pStateMan->m_pClient->Connect( cl_ip->GetString(), cl_port->GetInt() );
		m_pStateMan->PushState( std::make_unique<CStatePlay>() );
	}

	if ( pStateMan->m_pGui->LabelButton( 2, "Quit", pStateMan->m_pGui->m_vScreenCentre - CVector(0,2), CVector( 0.5, 0.5 ) ) )
	{
		pStateMan->PopState();
	}
}