#include "gamestates.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cvar_clientside.hpp"

void CStatePlay::Enter()
{
	m_projectionMatrix = glm::perspective( glm::radians( fov->GetFloat() ),
											 scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f );
	m_screenMatrix	 = glm::ortho( 0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat() );

	m_pDiffuseShader = shaderSystem::LoadShader("shaders/generic.vert", "shaders/generic.frag");

	m_pTerrainPNG = materialSystem::LoadTexture("terrain.png");

	CGameStateMachine* pStateMan = reinterpret_cast<CGameStateMachine*>(m_pStateMan);

	m_pLocalPlayer = new CEntityPlayer();
	m_pLocalWorld = new CWorld( m_pDiffuseShader, m_pDiffuseShader );

	m_pLocalPlayer->m_pInputMan = pStateMan->m_pGui->m_pInputMan;
	
	m_pLocalWorld->AddEntity(m_pLocalPlayer);
}

void CStatePlay::Exit()
{
	delete m_pLocalPlayer;
	delete m_pLocalWorld;
}

void CStatePlay::Update()
{
	CGameStateMachine* pStateMan = reinterpret_cast<CGameStateMachine*>(m_pStateMan);

	// Should we leave the game?
	bool leave = false;

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
									m_pLocalWorld->m_iTimeOfDay, CVector(0,1,0) );

		pStateMan->m_pInputMan->m_bInGui = false;

		// glDisable( GL_DEPTH_TEST ); // Skybox
		// {
		// 	skyboxModel.m_vPosition = m_pLocalPlayer->m_camera.m_vPosition;
		// 	skyboxModel.Render();

		// 	skyboxSunModel->m_vPosition = skyboxModel.m_vPosition;
		// 	skyboxSunModel->m_vRotation = CVector( 0, 0, -sunAngle );
		// 	skyboxSunModel->Render();
		// }
		// glEnable( GL_DEPTH_TEST );

		glClear(GL_COLOR_BUFFER_BIT);

		m_pLocalPlayer->UpdateClient( m_pLocalWorld );

		m_pDiffuseShader->Use();

		glBindTexture( GL_TEXTURE_2D, m_pTerrainPNG->m_iId );

		m_pLocalWorld->Render();

		BlockTexture bTex	  = GetDefaultBlockTextureSide( m_pLocalPlayer->m_iSelectedBlockType, Direction::NORTH );
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