#include "entities/entityplayer.hpp"
#include "gui/gui.hpp"
#include "inventory/inventory.hpp"
#include "localization/localizer.hpp"
#include "network/client.hpp"
#include "packs.hpp"
#include "particles/particlesystem.hpp"
#include "rendering/modelmanager.hpp"
#include "rendering/shadermanager.hpp"
#include "rendering/texturemanager.hpp"
#include "sdlstuff/sdlwindow.hpp"
#include "shared/statemanager.hpp"
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
	CLocalizer *m_pLocalizer   = nullptr;

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
	CShader *m_pChunkShader;

	CModel m_skyboxModel;
	CModel m_cloudModel;
	CModel m_selectionBox;
	CModel *m_pStellarModel;

	Vector4f m_cloudOffset;

	CParticleManager m_particleMan;

	CTexture *m_pTerrainPNG;
	CTexture *m_pHotbarTex;
	CTexture *m_pHotbarSelectTex;
	CTexture *m_pWaterOverlay;

	CInventory *m_invCreative;

	bool m_bInPause		= false;
	bool m_bInChat		= false;
	bool m_bDebugScreen = false;

	int64_t m_iLastTick = 0;

	float m_fSunAngle = 76.65f;
	float m_fFPS = 0.0f;
};

class CStateMenu : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();

	std::vector<std::string> m_splashes;
	std::string m_splash;
};

class CStateOptionsMenu : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();

	int m_iVolumeSlider	 = 100;
	bool m_bEnableReverb = true;
};
class CKickScreen : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();
};

class CStatePackMenu : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();

	std::vector<resourcePacks::packInfo> m_packList;
	std::vector<bool> m_packEnabled;
	int m_iScroll = 0;
};

class CStateLanguageMenu : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();

	std::vector<std::pair<std::string, std::string>> m_languageList;
	int m_iScroll = 0;
};

class CStateJoinMenu : public CState
{
	void Enter();
	void ReturnedTo();
	void Exit();
	void Update();
};