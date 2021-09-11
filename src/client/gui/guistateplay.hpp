#include "guistate.hpp"
#include "rendering/texturemanager.hpp"

#pragma once

class CGuiStatePlay : public CGuiState
{
  public:
	CGuiStatePlay();

	unsigned int Frame( CGui *pGui, CWorld *pLocalWorld );

	enum State { STATE_NORMAL = 0x0, STATE_CHATTING = 0x1, STATE_PAUSED = 0x2 };

  private:
	CTexture *m_pCrossHairTex = nullptr;
	CTexture *m_pTerrainTex	  = nullptr;
};