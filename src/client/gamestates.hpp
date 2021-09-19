#include "shared/statemanager.hpp"
#include "gui/gui.hpp"
#include "network/client.hpp"

#pragma once

// Like CStateMachine but holds extra client-y stuff
class CGameStateMachine : public CStateMachine
{
	public:
		CNetworkClient *m_pClient;
		CGui *m_pGui;
};

class CStatePlay : public CState
{
	void Enter();
	void Exit();
	void Update();
};