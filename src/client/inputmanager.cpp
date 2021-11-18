#include "inputmanager.hpp"

// Usually set/handled by the window on the client
// Or by the network in the case of the server

CInputManager::CInputManager()
{
	// Fill the array
	for ( int i = 0; i < sizeof( m_bKeyboardState ) / sizeof( bool ); i++ )
		m_bKeyboardState[i] = false;
	for ( int i = 0; i < sizeof( m_bInputState ) / sizeof( bool ); i++ )
		m_bInputState[i] = false;
}
CInputManager::~CInputManager() {}

void CInputManager::Update()
{
	for ( int i = 0; i < INKEY_LAST; i++ )
	{
		m_bOldInputState[i] = m_bInputState[i];
		m_bInputState[i]	= false;
	}

	// TODO: Custom Controls (ConVars?)

	m_bInputState[INKEY_FRONT] = m_bOldKeyboardState['W'];
	m_bInputState[INKEY_LEFT]  = m_bOldKeyboardState['A'];
	m_bInputState[INKEY_BACK]  = m_bOldKeyboardState['S'];
	m_bInputState[INKEY_RIGHT] = m_bOldKeyboardState['D'];

	m_bInputState[INKEY_UP]	  = m_bOldKeyboardState[' '];
	m_bInputState[INKEY_DOWN] = m_bOldKeyboardState[KBD_SHIFT];

	m_bInputState[INKEY_CHAT] = m_bOldKeyboardState['T'];
	m_bInputState[INKEY_OUT]  = m_bOldKeyboardState[KBD_ESCAPE];

	m_bInputState[INKEY_FLY] = m_bOldKeyboardState['V'];

	m_bInputState[INKEY_INV] = m_bOldKeyboardState['E'];
}