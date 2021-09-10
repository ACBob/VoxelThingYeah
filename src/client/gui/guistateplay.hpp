#include "guistate.hpp"

#pragma once

class CGuiStatePlay : public CGuiState
{
	public:
		CGuiStatePlay();

		unsigned int Frame(CGui *pGui, CWorld *pLocalWorld);

		enum State
		{
			STATE_NORMAL = 0x0,
			STATE_CHATTING = 0x1,
			STATE_PLAYING = 0x2
		};
};