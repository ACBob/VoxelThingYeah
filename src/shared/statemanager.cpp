#include "statemanager.hpp"

void CStateMachine::PushState( std::unique_ptr<CState> state )
{
	m_stateStack.push_back( std::move( state ) );
	m_stateStack.back()->m_pStateMan = this;
	m_stateStack.back()->Enter();
}

void CStateMachine::PopState()
{
	if ( !m_stateStack.empty() )
	{
		m_stateStack.back()->Exit();
		m_stateStack.pop_back();
		if ( !m_stateStack.empty() )
			m_stateStack.back()->ReturnedTo();
	}
}

void CStateMachine::Flush()
{
	while ( !m_stateStack.empty() )
		PopState();
}

void CStateMachine::Update()
{
	if ( !m_stateStack.empty() )
		m_stateStack.back()->Update();
}

bool CStateMachine::IsEmpty() { return m_stateStack.empty(); }