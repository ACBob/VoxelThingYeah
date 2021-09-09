#include "gui.hpp"

#include "world/world.hpp"
#include "entities/entityplayer.hpp"

#pragma once

class CGuiStateManager 
{
	enum GuiState
	{
		GUISTATE_PLAY = 0x00, // State when we're playing, i.e hotbar and stuff
	};
	enum GuiSubState
	{
		// GUISTATE_PLAY
			PLAY_NONE = 0x00, // When we're running around playing
			PLAY_CHAT = 0x01, // When we're chatting, still displays the same elements as NONE, just with chat
			PLAY_PAUSE = 0x02, // When we're paused
	};

	public:
		
		CGuiStateManager();
		~CGuiStateManager();

		void Update(CEntityPlayer *pLocalPlayer, CWorld *pLocalWorld);

		GuiState m_state = GUISTATE_PLAY;
		GuiSubState m_subState = PLAY_NONE;

		CInputManager *m_pInputManager = nullptr;
		CGui *m_pGui = nullptr;
};