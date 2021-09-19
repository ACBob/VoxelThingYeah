#include "statemanager.hpp"

void CStateMachine::PushState(std::unique_ptr<CState> state)
{
	state->Enter();
	state->m_pStateMan = this;
	m_stateStack.push_back(std::move(state));
}

void CStateMachine::PopState()
{
	if (!m_stateStack.empty())
	{
		m_stateStack.back()->Exit();
		m_stateStack.pop_back();
	}
}

void CStateMachine::Flush()
{
	while (!m_stateStack.empty())
		PopState();
}

void CStateMachine::Update()
{
	if (!m_stateStack.empty())
		m_stateStack.back()->Update();
}