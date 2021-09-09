#include "guistatemanager.hpp"

CGuiStateManager::CGuiStateManager() {}
CGuiStateManager::~CGuiStateManager() {}

void CGuiStateManager::Update()
{
	switch (m_state)
	{
		case GUISTATE_PLAY:
		{

			switch(m_subState)
			{
				case PLAY_CHAT:
				case PLAY_NONE:
					m_pGui->Label("Lol Poo Fart", CVector(0,0));
				break;

				case PLAY_PAUSE:
				break;
			}
		}
		break;
	}

	m_pGui->Update();
}