#include "gui.hpp"

#include "world/world.hpp"
#include "entities/entityplayer.hpp"

#pragma once

#include "guistate.hpp"
#include "guistateplay.hpp"
#include "guistateoptions.hpp"

class CGuiStateManager 
{
	public:		
		CGuiStateManager();
		~CGuiStateManager();

		void Update(CWorld *pLocalWorld);

		CInputManager *m_pInputManager = nullptr;
		CGui *m_pGui = nullptr;

		enum GuiState
		{
			GUISTATE_PLAY = 0x0,
			GUISTATE_OPTIONS = 0x1,
		};

		unsigned int m_iGuiState = 0;
		CGuiState *m_pGuiState = nullptr;

		CGuiStatePlay m_statePlay;
		CGuiStateOptions m_stateOpt;
};