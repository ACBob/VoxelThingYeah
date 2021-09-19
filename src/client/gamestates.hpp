#include "shared/statemanager.hpp"
#include "gui/gui.hpp"
#include "network/client.hpp"
#include "entities/entityplayer.hpp"
#include "world/world.hpp"
#include "rendering/shadermanager.hpp"
#include "rendering/texturemanager.hpp"
#include "sdlstuff/sdlwindow.hpp"
#include "rendering/modelmanager.hpp"

#pragma once

// Like CStateMachine but holds extra client-y stuff
class CGameStateMachine : public CStateMachine
{
	public:
		CNetworkClient *m_pClient;
		CGui *m_pGui;
		CGameWindow *m_pWindow;
		CInputManager *m_pInputMan;
};

class CStatePlay : public CState
{
	void Enter();
	void Exit();
	void Update();

	CEntityPlayer *m_pLocalPlayer;
	CWorld *m_pLocalWorld;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_screenMatrix;

	CShader *m_pDiffuseShader;
	CShader *m_pUnlitShader;
	CShader *m_pSkyboxShader;

	CModel m_skyboxModel;
	CModel *m_pStellarModel;

	CTexture *m_pTerrainPNG;
};

class CStateMenu : public CState
{
	void Enter();
	void Exit();
	void Update();
};