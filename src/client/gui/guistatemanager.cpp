#include "guistatemanager.hpp"

#include "cvar_clientside.hpp"

CGuiStateManager::CGuiStateManager()
{
	m_pGuiState = &m_statePlay;
}
CGuiStateManager::~CGuiStateManager() {}

void CGuiStateManager::Update(CWorld *pLocalWorld)
{
	unsigned int nextState = m_pGuiState->Frame(m_pGui, pLocalWorld);
	if (m_iGuiState != nextState)
	{
		m_iGuiState = nextState;
		
		switch (nextState)
		{
			case GUISTATE_PLAY:
				m_pGuiState = &m_statePlay;
			break;
			case GUISTATE_OPTIONS:
				m_pGuiState = &m_stateOpt;
			break;
		}

		m_pGuiState->m_iState = 0; // Reset the gui
	}
	m_pGui->Update();
}