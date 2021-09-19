#include "gamestates.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rendering/modelloader.hpp"

#include "cvar_clientside.hpp"

void CStatePlay::Enter()
{
	CGameStateMachine* pStateMan = reinterpret_cast<CGameStateMachine*>(m_pStateMan);


	m_projectionMatrix = glm::perspective( glm::radians( fov->GetFloat() ),
											 scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f );
	m_screenMatrix	 = glm::ortho( 0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat() );

	m_pDiffuseShader = shaderSystem::LoadShader("shaders/generic.vert", "shaders/generic.frag");
	m_pUnlitShader = shaderSystem::LoadShader("shaders/generic.vert", "shaders/unlit.frag");
	m_pSkyboxShader = shaderSystem::LoadShader("shaders/sky.vert", "shaders/sky.frag");

	GetCubeModel( m_skyboxModel, CVector(-5, -5, -5) );
	m_pStellarModel = modelSystem::LoadModel("sun.obj");

	m_pStellarModel->SetShader(m_pUnlitShader);
	m_skyboxModel.SetShader(m_pSkyboxShader);

	m_pTerrainPNG = materialSystem::LoadTexture("terrain.png");

	m_pLocalPlayer = new CEntityPlayer();
	m_pLocalWorld = new CWorld( m_pDiffuseShader, m_pDiffuseShader );

	m_pLocalPlayer->m_pInputMan = pStateMan->m_pGui->m_pInputMan;
	
	m_pLocalWorld->AddEntity(m_pLocalPlayer);

	pStateMan->m_pClient->m_pLocalPlayer = m_pLocalPlayer;
	pStateMan->m_pClient->m_pLocalWorld = m_pLocalWorld;
}

void CStatePlay::Exit()
{
	CGameStateMachine* pStateMan = reinterpret_cast<CGameStateMachine*>(m_pStateMan);

	delete m_pLocalPlayer;
	delete m_pLocalWorld;

	pStateMan->m_pClient->m_pLocalPlayer = nullptr;
	pStateMan->m_pClient->m_pLocalWorld = nullptr;
}

void CStatePlay::Update()
{
	CGameStateMachine* pStateMan = reinterpret_cast<CGameStateMachine*>(m_pStateMan);

	// Should we leave the game?
	bool leave = false;

	float fSunAngle   = 180 * ( 1 - ( m_pLocalWorld->m_iTimeOfDay / 12000.0f ) );
	CVector vSunForward = CVector( 0, 1, 0 ).Rotate( 3, fSunAngle );

	if ( !pStateMan->m_pClient->m_bConnected )
		leave = true;
	else
	{
		CVector f			= m_pLocalPlayer->m_camera.GetForward();
		CVector v			= m_pLocalPlayer->m_camera.m_vPosition + f;
		glm::mat4 view = glm::lookAt(
			glm::vec3( m_pLocalPlayer->m_camera.m_vPosition.x, m_pLocalPlayer->m_camera.m_vPosition.y, m_pLocalPlayer->m_camera.m_vPosition.z ),
			glm::vec3( v.x, v.y, v.z ), glm::vec3( VEC_UP.x, VEC_UP.y, VEC_UP.z ) );
		shaderSystem::SetUniforms( view, m_projectionMatrix, m_screenMatrix, pStateMan->m_pWindow->GetMS(),
									m_pLocalWorld->m_iTimeOfDay, vSunForward );

		pStateMan->m_pInputMan->m_bInGui = false;

		glDisable( GL_DEPTH_TEST ); // Skybox
		{
			m_skyboxModel.m_vPosition = m_pLocalPlayer->m_camera.m_vPosition;
			m_skyboxModel.Render();

			m_pStellarModel->m_vPosition = m_skyboxModel.m_vPosition;
			m_pStellarModel->m_vRotation = CVector( 0, 0, -fSunAngle );
			m_pStellarModel->Render();
		}
		glEnable( GL_DEPTH_TEST );

		m_pLocalPlayer->UpdateClient( m_pLocalWorld );

		m_pDiffuseShader->Use();

		glBindTexture( GL_TEXTURE_2D, m_pTerrainPNG->m_iId );

		m_pLocalWorld->Render();

		m_pLocalWorld->WorldTick( pStateMan->m_iTick );
		protocol::SendClientPlayerPos( pStateMan->m_pClient->m_pPeer, m_pLocalPlayer->m_vPosition, m_pLocalPlayer->m_vRotation );

		BlockTexture bTex = GetDefaultBlockTextureSide( m_pLocalPlayer->m_iSelectedBlockType, Direction::NORTH );
	}

	if (leave)
		m_pStateMan->PopState();
}


void CStateMenu::Enter()
{

}

void CStateMenu::Exit()
{

}

void CStateMenu::Update()
{
	CGameStateMachine* pStateMan = reinterpret_cast<CGameStateMachine*>(m_pStateMan);

	pStateMan->m_pInputMan->m_bInGui = true;

	pStateMan->m_pGui->Image(pStateMan->m_pGui->m_pBGTex, CVector(0,0), pStateMan->m_pGui->m_vScreenDimensions / pStateMan->m_pGui->m_iGuiUnit, CVector(0,0), CVector(0.5, 0.5, 0.5));

	if (pStateMan->m_pGui->LabelButton(1, "Play", pStateMan->m_pGui->m_vScreenCentre, CVector(0.5, 0.5)))
	{
		pStateMan->m_pClient->Connect(cl_ip->GetString(), cl_port->GetInt());
		m_pStateMan->PushState(std::make_unique<CStatePlay>());
	}
}