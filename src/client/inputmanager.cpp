#include "inputmanager.hpp"

// Usually set/handled by the window on the client
// Or by the network in the case of the server

#include <cstring>

CInputManager::CInputManager()
{
	// Fill the array
	memset( m_bInputState, false, sizeof( m_bInputState ) );
    memset( m_bOldInputState, false, sizeof( m_bOldInputState ) );
    memset( m_bKeyboardState, false, sizeof( m_bKeyboardState ) );
    memset( m_bOldKeyboardState, false, sizeof( m_bOldKeyboardState ) );

}
CInputManager::~CInputManager() {}

void CInputManager::Update()
{
    // Update the old input state
    memcpy( m_bOldInputState, m_bInputState, sizeof( m_bInputState ) );
    memset( m_bInputState, false, sizeof( m_bInputState ) );

	// TODO: Custom Controls (ConVars?)

	m_bInputState[CONTROLS_FRONT] = m_bOldKeyboardState['W'];
	m_bInputState[CONTROLS_LEFT]  = m_bOldKeyboardState['A'];
	m_bInputState[CONTROLS_BACK]  = m_bOldKeyboardState['S'];
	m_bInputState[CONTROLS_RIGHT] = m_bOldKeyboardState['D'];

	m_bInputState[CONTROLS_UP]	  = m_bOldKeyboardState[' '];
	m_bInputState[CONTROLS_DOWN] = m_bOldKeyboardState[KBD_SHIFT];

	m_bInputState[CONTROLS_CHAT] = m_bOldKeyboardState['T'];
	m_bInputState[CONTROLS_OUT]  = m_bOldKeyboardState[KBD_ESCAPE];

	m_bInputState[CONTROLS_FLY] = m_bOldKeyboardState['V'];

	m_bInputState[CONTROLS_INV] = m_bOldKeyboardState['E'];
}