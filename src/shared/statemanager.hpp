// Inspired by http://funwithbits.net/blog/handling-game-states/
// A generic state manager, suited towards games.
// The GUI also uses this.

#include <memory>
#include <vector>

#pragma once

class CStateMachine; // Forward decl

class CState
{
  public:
	virtual void Enter()  = 0; // Newly Entered
	virtual void ReturnedTo() = 0; // A state exited into us
	virtual void Exit()	  = 0; // We're closing
	virtual void Update() = 0; // Frame in us

	CStateMachine *m_pStateMan = nullptr;
};

class CStateMachine
{
	std::vector<std::unique_ptr<CState>> m_stateStack;

  public:
	// Pushes a new state onto the stack
	void PushState( std::unique_ptr<CState> state );
	// Pops the topmost state on the stack
	void PopState();
	// Exits all current states, if any
	void Flush();
	// Updates
	void Update();
	// Returns if the state is empty
	bool IsEmpty();
};