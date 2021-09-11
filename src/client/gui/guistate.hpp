#include "gui.hpp"
#include "world/world.hpp"

#pragma once

class CGuiState
{
  public:
	CGuiState();
	~CGuiState();

	// We also hand it a pointer to the world
	// So it can act on information.
	// Returns the state it wants to switch to, if any.
	virtual unsigned int Frame( CGui *pGui, CWorld *pWorld ) = 0;
	unsigned int m_iState									 = 0;
};