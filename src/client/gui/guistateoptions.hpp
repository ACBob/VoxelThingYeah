#include "guistate.hpp"
#include "rendering/texturemanager.hpp"

#pragma once

class CGuiStateOptions : public CGuiState
{
	public:
		CGuiStateOptions();

		unsigned int Frame(CGui *pGui, CWorld *pLocalWorld);

		enum State
		{
			STATE_NORMAL = 0x0
		};

	private:
};