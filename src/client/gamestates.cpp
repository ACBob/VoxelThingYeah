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