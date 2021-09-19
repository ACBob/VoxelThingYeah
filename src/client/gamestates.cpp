#include "gamestates.hpp"

void CStatePlay::Enter()
{

}

void CStatePlay::Exit()
{

}

void CStatePlay::Update()
{
	reinterpret_cast<CGameStateMachine*>(m_pStateMan)->m_pGui->Label("Hello World", CVector(0,0));
}


void CStateMenu::Enter()
{

}

void CStateMenu::Exit()
{

}

void CStateMenu::Update()
{
	CGameStateMachine* pStateMan = reinterpret_cast<CGameStateMachine*>(m_pStateMan);

	pStateMan->m_pGui->Image(pStateMan->m_pGui->m_pBGTex, CVector(0,0), pStateMan->m_pGui->m_vScreenDimensions / pStateMan->m_pGui->m_iGuiUnit, CVector(0,0), CVector(0.5, 0.5, 0.5));

	if (pStateMan->m_pGui->LabelButton(1, "Play", pStateMan->m_pGui->m_vScreenCentre, CVector(0.5, 0.5)))
		pStateMan->PushState(std::make_unique<CStatePlay>());
}