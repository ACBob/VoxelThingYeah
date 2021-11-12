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

#include "colours.hpp"

#include "blocks/blockbase.hpp"

#include "config.h"

const CModel::Vertex cloudPlane[4] = {
	// POSITION            NORMAL            UV
	{ -512.0f,  0.0f, -512.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
	{  512.0f,  0.0f, -512.0f, 0.0f, 0.0f, 1.0f, 0.25f, 0.0f },
	{  512.0f,  0.0f,  512.0f, 0.0f, 0.0f, 1.0f, 0.25f, 0.25f },
	{ -512.0f,  0.0f,  512.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.25f } };
const CModel::Face cloudPlaneFaces[4] = { { 2, 1, 0 }, { 0, 3, 2 }, { 0, 1, 2 }, { 2, 3, 0 } };

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

	GetCubeModel( m_skyboxModel, Vector3f( -5, -5, -5 ) );
	m_pStellarModel = modelSystem::LoadModel( "sun.obj" );

	m_pStellarModel->SetShader( m_pUnlitShader );
	m_skyboxModel.SetShader( m_pSkyboxShader );

	std::copy( cloudPlane, cloudPlane + 4, std::back_inserter( m_cloudModel.m_vertices ) );
	std::copy( cloudPlaneFaces, cloudPlaneFaces + 4, std::back_inserter( m_cloudModel.m_faces ) );
	m_cloudModel.Update();
	m_cloudModel.SetShader(m_pDiffuseShader);
	m_cloudModel.SetTexture(materialSystem::LoadTexture("clouds.png"));

	m_pStellarModel->SetTexture( materialSystem::LoadTexture( "sun.png" ) );

	m_pTerrainPNG	   = materialSystem::LoadTexture( "terrain.png" );
	m_pHotbarTex	   = materialSystem::LoadTexture( "hotbar.png" );
	m_pHotbarSelectTex = materialSystem::LoadTexture( "hotbar-selected.png" );

	m_pLocalPlayer = new CEntityPlayer();
	m_pLocalWorld  = new CWorld( m_pChunkShader, m_pDiffuseShader, m_pWaterShader, m_pTerrainPNG );

	m_pLocalPlayer->m_pInputMan = pStateMan->m_pInputMan;
	m_pLocalPlayer->m_pClient	= pStateMan->m_pClient;

	m_pLocalWorld->AddEntity( m_pLocalPlayer );

	pStateMan->m_pClient->m_pLocalPlayer = m_pLocalPlayer;
	pStateMan->m_pClient->m_pLocalWorld	 = m_pLocalWorld;
	pStateMan->m_pClient->m_pParticleMan = &m_particleMan;


	m_invCreative = new CInventory({
		new CBlockItem(64, STONE),
		new CBlockItem(64, DIRT),
		new CBlockItem(64, GRASS),
		new CBlockItem(64, COBBLE),
		new CBlockItem(64, PLANKS),
		new CBlockItem(64, BEDROCK),
		new CBlockItem(64, GLASS),
		new CBlockItem(64, LOG	),
		new CBlockItem(64, LEAVES),
		new CBlockItem(64, WATERSRC),
		new CBlockItem(64, LAVASRC),
		new CBlockItem(64, ORE_COAL),
		new CBlockItem(64, ORE_IRON),
		new CBlockItem(64, FLOWER),
		new CBlockItem(64, SAND),
		new CBlockItem(64, SANDSTONE),
		new CBlockItem(64, BRICKS),
		new CBlockItem(64, LGRASS),
		new CBlockItem(64, MOSSCBBLE),
		new CBlockItem(64, SNOW),
		new CBlockItem(64, SNOWGRASS),
		new CBlockItem(64, ICE),
		new CBlockItem(64, LIGHT_YELLOW),
		new CBlockItem(64, LIGHT_WHITE),
		new CBlockItem(64, LIGHT_RED),
		new CBlockItem(64, LIGHT_GREEN),
		new CBlockItem(64, LIGHT_BLUE),

		new CBlockItem(64, WOOL, DyePalette[0]),
		new CBlockItem(64, WOOL, DyePalette[1]),
		new CBlockItem(64, WOOL, DyePalette[2]),
		new CBlockItem(64, WOOL, DyePalette[3]),
		new CBlockItem(64, WOOL, DyePalette[4]),
		new CBlockItem(64, WOOL, DyePalette[5]),
		new CBlockItem(64, WOOL, DyePalette[6]),
		new CBlockItem(64, WOOL, DyePalette[7]),
		new CBlockItem(64, WOOL, DyePalette[8]),
		new CBlockItem(64, WOOL, DyePalette[9]),
		new CBlockItem(64, WOOL, DyePalette[10]),
		new CBlockItem(64, WOOL, DyePalette[11]),
		new CBlockItem(64, WOOL, DyePalette[12]),
		new CBlockItem(64, WOOL, DyePalette[13]),
		new CBlockItem(64, WOOL, DyePalette[14]),
		new CBlockItem(64, WOOL, DyePalette[15]),

		new CBlockItem(64, SOUNDMAKER),
	}, true);
}
void CStatePlay::ReturnedTo() {}

void CStatePlay::Exit()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	delete m_pLocalPlayer;
	delete m_pLocalWorld;
	delete m_invCreative;

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
	Vector3f vSunForward = Vector3f( 0, 1, 0 ).RotateAxis(0, cl_sunroll->GetFloat() * DEG2RAD).RotateAxis(1, cl_sunyaw->GetFloat() * DEG2RAD).RotateAxis( 2, m_fSunAngle  * DEG2RAD);

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

		soundSystem::SetListener( m_pLocalWorld, m_pLocalPlayer->m_camera.m_vPosition, m_pLocalPlayer->m_camera.GetForward(),
								  m_pLocalPlayer->m_vVelocity );

		// -----------------------
		// Rendering
		// -----------------------
		Vector3f f = m_pLocalPlayer->m_camera.GetForward();
		Vector3f v = m_pLocalPlayer->m_camera.m_vPosition + f;
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
			m_pStellarModel->m_vRotation = Vector3f( cl_sunroll->GetFloat(), cl_sunyaw->GetFloat(), -m_fSunAngle );
			m_pStellarModel->Render();
		}
		glEnable( GL_DEPTH_TEST );

		m_cloudModel.m_vPosition = m_skyboxModel.m_vPosition;
		m_cloudModel.m_vPosition.y = 256;
		m_cloudOffset += Vector4f(0, 0, .0015, 0) * pStateMan->m_fDelta;
		{
			Vector4f p;
			p.z = m_pLocalPlayer->m_vPosition.x / 2048;
			p.w = m_pLocalPlayer->m_vPosition.z / 2048;
			p.x = 1;
			p.y = 1;

			m_cloudModel.m_vUvOffset = m_cloudOffset + p;
		}
		m_cloudModel.Render();
		

		m_pDiffuseShader->Use();

		glBindTexture( GL_TEXTURE_2D, m_pTerrainPNG->m_iId );

		m_pLocalWorld->Render();

		// Particles Last
		m_particleMan.Render( m_pLocalPlayer->m_camera.m_vRotation );

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

		pStateMan->m_pGui->Label("Meegreef " MEEGREEF_VERSION, {0,1});

		int hours	= m_pLocalWorld->m_iTimeOfDay / 1000;
		int minutes = ( m_pLocalWorld->m_iTimeOfDay - ( hours * 1000 ) ) / 16.6666;
		snprintf( guiBuf, 100, "%02i:%02i", hours, minutes );
		pStateMan->m_pGui->Label( guiBuf, {pStateMan->m_pGui->m_vGUICentre.x, 1}, 1.0f, {255, 255, 255}, CGui::TEXTALIGN_CENTER);
		
		// F3 screen
		if (pStateMan->m_pInputMan->m_bKeyboardState[KBD_F3] && !pStateMan->m_pInputMan->m_bOldKeyboardState[KBD_F3])
			m_bDebugScreen = !m_bDebugScreen;

		if (m_bDebugScreen)
		{
			// position
			snprintf( guiBuf, 100, "Position: <%.2f, %.2f, %.2f>", m_pLocalPlayer->m_vPosition.x, m_pLocalPlayer->m_vPosition.y, m_pLocalPlayer->m_vPosition.z );
			pStateMan->m_pGui->Label( guiBuf, Vector3f( 0, 3 ) );

			// velocity
			snprintf( guiBuf, 100, "Velocity: <%.2f, %.2f, %.2f>", m_pLocalPlayer->m_vVelocity.x, m_pLocalPlayer->m_vVelocity.y, m_pLocalPlayer->m_vVelocity.z );
			pStateMan->m_pGui->Label( guiBuf, Vector3f( 0, 4 ) );

			// Pointed block id/data
			// Data displayed in hex
			if (m_pLocalPlayer->m_pointed.m_pBlock != nullptr)
			{
				snprintf( guiBuf, 100, "Pointed block: %i, %04X", m_pLocalPlayer->m_pointed.m_pBlock->m_iBlockType, m_pLocalPlayer->m_pointed.m_pBlock->m_iBlockData );
				pStateMan->m_pGui->Label( guiBuf, Vector3f( 0, 5 ) );
			}
			else
			{
				pStateMan->m_pGui->Label( "Pointed block: null", Vector3f( 0, 5 ) );
			}
		}

		// Hot-bar
		pStateMan->m_pGui->ImageCentered( {pStateMan->m_pGui->m_vGUICentre.x, -2.125f}, {18.5, 2.5}, pStateMan->m_pGui->m_pHotbarTex );

		float p;
		for ( int i = 0; i < 8; i++ )
		{
			p = 8.0f * ( i / 4.0f - 1.0f );

			if ( m_pLocalPlayer->m_inventory.Slot( i ) == nullptr ||
				 m_pLocalPlayer->m_inventory.Slot( i )->GetCount() == 0 )
				continue;
			
			pStateMan->m_pGui->ItemCentered( 0, { pStateMan->m_pGui->m_vGUICentre.x + p, -2.125f }, {2,2}, m_pLocalPlayer->m_inventory.Slot( i ) );
		}
		p = 8.0f * ( m_pLocalPlayer->m_iSelectedItemIDX / 4.0f - 1.0f );
		pStateMan->m_pGui->ImageCentered( { pStateMan->m_pGui->m_vGUICentre.x + p, -2.125f }, {3,3}, pStateMan->m_pGui->m_pHotbarSelectTex );

		if ( m_bInPause )
		{
			if ( pStateMan->m_pGui->LabelButtonCentered( GUIGEN_ID, pStateMan->m_pLocalizer->GetString("gui.dismiss_far"), pStateMan->m_pGui->m_vGUICentre, {24, 2} ) )
			{
				m_bInPause = false;
			}
			if ( pStateMan->m_pGui->LabelButtonCentered( GUIGEN_ID, pStateMan->m_pLocalizer->GetString("gui.title.options"), { pStateMan->m_pGui->m_vGUICentre.x, -12 }, {24, 2} ) )
			{
				pStateMan->PushState( std::make_unique<CStateOptionsMenu>() );
			}
			if ( pStateMan->m_pGui->LabelButtonCentered( GUIGEN_ID, pStateMan->m_pLocalizer->GetString("gui.disconnect"), { pStateMan->m_pGui->m_vGUICentre.x, -6 }, {24, 2} ) )
			{
				pStateMan->m_pClient->Disconnect();
				pStateMan->PopState();
			}
		}
		else if (m_pLocalPlayer->m_bInInventory)
		{
			pStateMan->m_pGui->Image9PatchCentered( pStateMan->m_pGui->m_vGUICentre, {18, 19}, 5, pStateMan->m_pGui->m_pWindowTex );
			pStateMan->m_pGui->Label( pStateMan->m_pLocalizer->GetString("gui.inventory"), pStateMan->m_pGui->m_vGUICentre - Vector3f(8, 8), 1.0f, {255, 255, 255});

			int s = pStateMan->m_pGui->Inventory( pStateMan->m_pGui->m_vGUICentre - Vector3f(8, 6.5), 8, m_invCreative );
			if (s != -1)
			{
				CItem *pHeldItem = m_pLocalPlayer->m_inventory.Slot( m_pLocalPlayer->m_iSelectedItemIDX );
				pHeldItem->SetCount( m_invCreative->Take( s, 64 ) );
				pHeldItem->SetID( m_invCreative->Slot( s )->GetID() );
				// TODO: Assuming block item, this is a hack
				((CBlockItem*)pHeldItem)->SetData( ((CBlockItem*)m_invCreative->Slot( s ))->GetData() );
			}
		}
		else
		{
			pStateMan->m_pGui->ImageCentered( pStateMan->m_pGui->m_vGUICentre, {2, 2}, pStateMan->m_pGui->m_pCrosshairTex );
		}

		// Chat Rendering
		for ( int i = 5; i > 0; i-- )
		{
			if ( i > pStateMan->m_pClient->m_chatBuffer.size() )
				continue;

			pStateMan->m_pGui->Label(
				pStateMan->m_pClient->m_chatBuffer[pStateMan->m_pClient->m_chatBuffer.size() - i].c_str(),
				Vector3f( 0, i + 1 ) );
		}

		if ( m_bInChat )
		{
			// Chat
			const char *chat = pStateMan->m_pGui->TextInput( 69, Vector3f( 0, 0 ) );

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

	soundSystem::SetListener( nullptr, Vector3f( 0, 0, 0 ), Vector3f( 0, 0, 1 ), Vector3f( 0, 0, 0 ) );
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
	pStateMan->m_pGui->ImageRepeating( {0,-1}, pStateMan->m_pGui->m_vGUISize, pStateMan->m_pGui->m_pGuiBGTex );

	pStateMan->m_pGui->ImageCentered( pStateMan->m_pGui->m_vGUICentre - Vector3f(0, 8), {45, 8.57}, pStateMan->m_pGui->m_pGuiTitleTex );

	if ( pStateMan->m_pGui->LabelButtonCentered( GUIGEN_ID, pStateMan->m_pLocalizer->GetString("gui.title.startgame"),
										pStateMan->m_pGui->m_vGUICentre, {32, 2} ) )
	{
		pStateMan->m_pClient->Connect( cl_ip->GetString(), cl_port->GetInt() );
		pStateMan->PushState( std::make_unique<CStatePlay>() );
	}

	if ( pStateMan->m_pGui->LabelButtonCentered( GUIGEN_ID, pStateMan->m_pLocalizer->GetString("gui.title.options"), { pStateMan->m_pGui->m_vGUICentre.x, -13 }, {32, 2} ) )
	{
		pStateMan->PushState( std::make_unique<CStateOptionsMenu>() );
	}

	if ( pStateMan->m_pGui->LabelButtonCentered( GUIGEN_ID, pStateMan->m_pLocalizer->GetString("gui.title.quit"), { pStateMan->m_pGui->m_vGUICentre.x, -6 }, {32, 2} ) )
	{
		pStateMan->PopState();
	}

	pStateMan->m_pGui->Label( "Meegreef " MEEGREEF_VERSION, {0, -1} );
	pStateMan->m_pGui->Label( "Copyright Baob Koiss.", {-1, -1}, 1.0f, {255, 255, 255}, CGui::TEXTALIGN_RIGHT );
}

void CStateOptionsMenu::Enter()
{
	CGameStateMachine *pStateMan = reinterpret_cast<CGameStateMachine *>( m_pStateMan );
	pStateMan->m_pGui->ClearBuffers();

	m_iVolumeSlider = cl_volume->GetFloat() * 100;
	m_bEnableReverb = cl_reverb->GetBool();

	soundSystem::SetListener( nullptr, Vector3f( 0, 0, 0 ), Vector3f( 0, 0, 1 ), Vector3f( 0, 0, 0 ) );
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
	pStateMan->m_pInputMan->m_bInGui = true;
	pStateMan->m_pGui->ImageRepeating( {0,-1}, pStateMan->m_pGui->m_vGUISize, pStateMan->m_pGui->m_pGuiBGTex );

	pStateMan->m_pGui->Label( pStateMan->m_pLocalizer->GetString("gui.title.options"), { pStateMan->m_pGui->m_vGUICentre.x, 3 }, 1.0f, {255, 255, 255}, CGui::TEXTALIGN_CENTER );

	if (pStateMan->m_pGui->LabelButtonCentered( GUIGEN_ID, pStateMan->m_pLocalizer->GetString("gui.dismiss"),
										pStateMan->m_pGui->m_vGUICentre + Vector3f( 0, 10 ), {16, 2} ) )
	{
		pStateMan->PopState();
	}

	pStateMan->m_pGui->Label( pStateMan->m_pLocalizer->GetStringFMT("gui.options.mastervolume", m_iVolumeSlider), { pStateMan->m_pGui->m_vGUICentre.x, 6 }, 1.0f, {255, 255, 255}, CGui::TEXTALIGN_CENTER );
	if (pStateMan->m_pGui->HorzSlider( GUIGEN_ID, { pStateMan->m_pGui->m_vGUICentre.x - 8, 8 }, {16, 2}, 100, m_iVolumeSlider ))
	{
		cl_volume->SetFloat( m_iVolumeSlider / 100.0f );
	}

	if (pStateMan->m_pGui->LabelButtonCentered( GUIGEN_ID, pStateMan->m_pLocalizer->GetString("gui.options.resourcepacks"),
										pStateMan->m_pGui->m_vGUICentre + Vector3f( 0, 7 ), {16, 2} ) )
	{
		pStateMan->PushState( std::make_unique<CStatePackMenu>() );
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

	pStateMan->m_pGui->ImageRepeating( {0,-1}, pStateMan->m_pGui->m_vGUISize, pStateMan->m_pGui->m_pGuiBGTex );

	pStateMan->m_pGui->Label( pStateMan->m_pLocalizer->GetString("gui.kicked"), { pStateMan->m_pGui->m_vGUICentre.x, 3 }, 1.0f, {255, 127, 127}, CGui::TEXTALIGN_CENTER );

	pStateMan->m_pGui->Label( pStateMan->m_pLocalizer->GetString(cl_kickreason->GetString()), pStateMan->m_pGui->m_vGUICentre, 1.0f, {255, 255, 255}, CGui::TEXTALIGN_CENTER );

	if (pStateMan->m_pGui->LabelButtonCentered( GUIGEN_ID, pStateMan->m_pLocalizer->GetString("gui.dismiss"),
										pStateMan->m_pGui->m_vGUICentre + Vector3f( 0, 10 ), {16, 2} ) )
	{
		pStateMan->PopState();
	}
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

	pStateMan->m_pGui->ImageRepeating( {0,-1}, pStateMan->m_pGui->m_vGUISize, pStateMan->m_pGui->m_pGuiBGTex );

	if (pStateMan->m_pGui->LabelButtonCentered(GUIGEN_ID, "Nobody here but us chickens!", pStateMan->m_pGui->m_vGUICentre, {24, 2}))
	{
		pStateMan->PopState();
	}
}