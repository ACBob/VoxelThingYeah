#include "entities/entityplayer.hpp"
#include "gui/gui.hpp"
#include "network/client.hpp"
#include "particles/particlesystem.hpp"
#include "rendering/modelmanager.hpp"
#include "rendering/shadermanager.hpp"
#include "rendering/texturemanager.hpp"
#include "sdlstuff/sdlwindow.hpp"
#include "shared/statemanager.hpp"
#include "sound/soundmanager.hpp"
#include "world/world.hpp"

#pragma once

// Like CStateMachine but holds extra client-y stuff
class CGameStateMachine : public CStateMachine
{
  public:
	CNetworkClient *m_pClient  = nullptr;
	CGui *m_pGui			   = nullptr;
	CGameWindow *m_pWindow	   = nullptr;
	CInputManager *m_pInputMan = nullptr;

	int64_t m_iTick = 0; // Client Tick
	float m_fDelta	= 0.0f;
};

class CStatePlay : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();

	CEntityPlayer *m_pLocalPlayer;
	CWorld *m_pLocalWorld;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_screenMatrix;

	CShader *m_pDiffuseShader;
	CShader *m_pUnlitShader;
	CShader *m_pSkyboxShader;
	CShader *m_pWaterShader;

	CModel m_skyboxModel;
	CModel *m_pStellarModel;

	CParticleManager m_particleMan;

	CTexture *m_pTerrainPNG;
	CTexture *m_pHotbarTex;
	CTexture *m_pHotbarSelectTex;

	bool m_bInPause = false;
	bool m_bInChat	= false;

	int64_t m_iLastTick = 0;
};

class CStateMenu : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();
};

class CStateOptionsMenu : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();

	int m_iVolumeSlider = 100;
};
class CKickScreen : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();
};