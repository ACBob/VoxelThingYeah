#include "guistatemanager.hpp"

#include "cvar_clientside.hpp"

CGuiStateManager::CGuiStateManager()
{
	m_pGuiState = &m_statePlay;
}
CGuiStateManager::~CGuiStateManager() {}

void CGuiStateManager::Update(CWorld *pLocalWorld)
{
	m_pGuiState->Frame(m_pGui, pLocalWorld);
	m_pGui->Update();
}